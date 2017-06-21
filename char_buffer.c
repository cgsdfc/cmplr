#include "char_buffer.h"
#include <string.h>
#include<stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "char_buffer.h"

extern char *inline_file;
typedef char_buffer *cb;
static int init_char_buffer(cb, FILE*,int,int);

static int count_chars(char *file)
{
  struct stat statbuf;
  if (stat(file, &statbuf)!=0)
  {
    return -1;
  }
  return statbuf.st_size;

}

static int count_lines(FILE *f)
{
  int ch;

  if (feof(f) || ferror(f)) 
  {
    perror ("count_lines");
    return -1;
  }
  for (int line=0, ch=fgetc(f);;ch=fgetc(f))
  {
    if (ch==EOF) {
      fseek(f,0,SEEK_SET);
      return line;
    }
    if (ch=='\n')
      line++;
  }
}

int init_char_buffer_from_string (char_buffer *buffer, char *string)
{
  if (string == NULL)
    return -1;
  FILE *ss;
  int chars,lines;

  do {
    chars=strlen (string);
    if (( ss=fmemopen(string,chars,"r")) <0 )
    {
      break;
    }
    lines=count_lines(ss);
    if(lines<0) {
      break;
    }
    return init_char_buffer (buffer, ss, chars,lines); 
  } while (0);

  perror("init_char_buffer_from_string");
  return -1;
}

int init_char_buffer_from_file (char_buffer *buffer, char *file)
{
  FILE *f;
  int lines=0;
  int chars=0;

  do {
    if ((f=fopen (file,"r"))==NULL)
    {
      break;
    }
    if ((chars=count_chars(file))<0)
    {
      break;
    }

    if ((lines=count_lines(f))<0)
    {
      break;
    }

    return init_char_buffer (buffer, f, chars, lines);
  } while (0);
  perror (file);
  return -1;

}


/** read all the char from file 
 * put the chars into `buf`
 * put the number of chars of each line 
 * into `limits`
 * put the number of lines into `lines`
 * put the number of chars into `end`
 * set `index` points to 1st char
 * set `pos` to { lineno=1, column=1 }
 */
static
int init_char_buffer (char_buffer *buffer, FILE *f, int chars, int lines)
{
  void *buf,*limits;
  char line[BUFSIZ];
  int i=0;

  do {
    if ((buf=malloc(sizeof (char) * ( chars + 5)))==NULL)
    {
      break;
    }

    if ((limits=malloc(sizeof (int) * (1 + lines)))==NULL)
    {
      break;
    }

    buffer->limits=(int*)limits;
    buffer->buf=(char*)buf;

    buffer->buf[0]=0;
    while (NULL != fgets (line, BUFSIZ, f))
    {
      buffer->limits[++i]=strlen (line);
      strncat (buffer->buf, line, BUFSIZ);
    }

    buffer->end=chars;
    buffer->index=0;
    buffer->lines=lines;
    buffer->pos.lineno=1;
    buffer->pos.column=1;
    return 0;
  } while (0);

  perror ("init_char_buffer");
  return -1;
}


int prev_char (char_buffer* buffer)
{
  if (buffer -> index == 0) 
    return EOF;
  return buffer -> buf [ buffer -> index - 1];
}

int next_char (char_buffer* buffer)
{
  if (buffer -> index == buffer -> end)
    return EOF;
  return buffer -> buf [buffer -> index + 1];

}


/** get next char 
 * on success it increases the char pointer, updates
 * position and returns the char before the pointer was
 * increased.
 * on EOF it returns EOF.
 */
int get_char (char_buffer *buffer)
{
  int _char = peek_char (buffer);
  if (_char == EOF)
    return EOF;

  ++buffer->index;
  ++buffer->pos.column;
  if (prev_char(buffer) == '\n')
  {
    buffer->pos.lineno++;
    buffer->pos.column=1;
  }

  return _char;
}

/** peek the current char 
 * if the buffer has been consumed and EOF has
 * been hit, it returns EOF;
 * otherwise it will fill the buffer
 * and returns the char at `index`
 */
int peek_char (char_buffer *buffer)
{
  return buffer -> index == buffer -> end ? EOF:
    buffer -> buf [ buffer -> index ];
}

/** put back the current char into the buffer
 * on success it decreases the `index` and
 * returns char at `index`;
 * on failure when `index` is 0 so no char can
 * be put back, it returns -1;
 */
int put_char (char_buffer *buffer)
{
  if (buffer->index == 0)
  {
    return EOF;
  }

  buffer->index--;
  buffer->pos.column--;
  int _char = peek_char (buffer);

  if (_char == '\n')
  {
    buffer->pos.lineno--;
    buffer->pos.column=
      buffer->limits[buffer->pos.lineno];
  }

  return _char;
}

int get_line_offset(char_buffer *buffer, int lineno)
{
  if (lineno < 1) {
    return -1;
  }
  if (lineno == 1) {
    return 0;
  }
  int offset=0;
  for (int i=1;i<lineno;++i)
  {
    offset += buffer->limits[i];
  }
  return offset;

}

int get_lineno (char_buffer *buffer)
{
  return buffer->pos.lineno;
}


char *peek_line (char_buffer *buffer, int lineno)
{
  static char buf[BUFSIZ];
  int limit;
  int offset;
  int i;
  int get_lineno(char_buffer*);
  int get_line_limit(char_buffer*,int);

  limit=get_line_limit(buffer,lineno);
  offset = get_line_offset (buffer, lineno);


  for (i=0;i<limit;++i)
  {
    buf[i]=buffer->buf[i+offset];
  }

  buf[i]=0;
  return buf;

}

int get_line_limit (char_buffer *buffer, int lineno)
{
  return buffer->limits[lineno];
}


void check_peek_line (char_buffer *buffer)
{
  int offset;
#if 0
  offset = get_line_offset(buffer, 1);
  assert (offset == 0);
  printf(peek_line(buffer, 1));

  offset = get_line_offset (buffer, 2);
  assert (offset == get_line_limit(buffer,1));
  printf(peek_line(buffer, 2));

  offset = get_line_offset (buffer, 3);
  assert (offset == get_line_limit(buffer,2) + get_line_limit(buffer, 1));
  printf(peek_line(buffer, 3));
#endif

  printf ("check_peek_line passed\n");
}

void check_char_buffer (void)
{
  void show_buffer (char_buffer*);
  char_buffer buffer;
  FILE *input;
  int r;
  int ch;
  char *file=inline_file;

  r=init_char_buffer_from_string (&buffer, file);
  assert (r==0);

  check_peek_line(&buffer);
  /* while ((ch=get_char (&buffer))!=EOF) */
  /* { */
  /*   putchar(ch); */
  /* } */


  printf("check_init_char_buffer passed\n");
}

void show_buffer (char_buffer *buffer)
{
  printf ("\"%s\"\n", buffer->buf);
  printf ("index = %d\n", buffer->index);
  printf ("char [index] = \'%c\'\n", buffer->buf[buffer->index]);
  printf ("lineno = %d, column = %d\n", buffer->pos.lineno, buffer->pos.column);

}

char *inline_file=
"\n"
"/* tokenizer.c */\n"
"#include <stdbool.h>\n"
"#include<stdlib.h>\n"
"#include <assert.h>\n"
"#include<stdio.h>\n"
"#include<string.h>\n"
"#include<ctype.h>\n"
"\n"
"\n"
"struct token;\n"
"static transfer_entry transfer_table[MAX_TRANSFER_ENTRIES][MAX_TRANSFER_ENTRIES];\n"
"\n"
"void init_transfer (void)\n"
"{\n"
"  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i) \n"
"  {\n"
"    for (int j=0;j<MAX_TRANSFER_ENTRIES;++j)\n"
"    {\n"
"      transfer_table[i][j].state = TK_NULL;\n"
"      transfer_table[i][j].tkz_act = append_token;\n"
"      transfer_table[i][j].char_class = "";\n"
"      transfer_table[i][j].is_accepted = false;\n"
"      transfer_table[i][j].is_reversed = false;\n"
"    }\n"
"  }\n"
"\n"
"}\n"
"\n"
"  \n"
"int add_transfer(int this_state, int state, bool is_accepted, bool is_reversed, char *char_class, tkz_action act)\n"
"{\n"
"  static int entry_counters[MAX_TRANSFER_ENTRIES]={0};\n"
"  int entry_counter = entry_counters[this_state];\n"
"  transfer_entry *this_entry = &transfer_table[this_state][entry_counter];\n"
"\n"
"  this_entry->state = state;\n"
"  this_entry->is_accepted = is_accepted;\n"
"  this_entry->is_reversed = is_reversed;\n"
"  this_entry->char_class = char_class;\n"
"  this_entry->tkz_act = act;\n"
"\n"
"  entry_counters[this_state]=entry_counter+1;\n"
"  return entry_counter;\n"
"\n"
"}\n"
"\n"
"bool can_transfer(transfer_entry *entry, int character) \n"
"{\n"
"  // TODO: delete it\n"
"  if (entry->char_class == NULL) \n"
"  {\n"
"    return false;\n"
"  }\n"
"  bool is_in_class = strchr (entry->char_class, character);\n"
"  return (is_in_class || entry->is_reversed);\n"
"\n"
"}\n"
"\n"
"tokenizer_state do_transfer (int state, int character, transfer_entry **entry)\n"
"{\n"
"  tokenizer_state next_state;\n"
"\n"
"  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)\n"
"  {\n"
"    next_state = transfer_table[state][i].state;\n"
"    *entry = &transfer_table[state][i];\n"
"    if (can_transfer(&transfer_table[state][i], character))\n"
"      break;\n"
"  }\n"
"\n"
"  return next_state;\n"
"\n"
"}\n"
"\n"
"transfer_entry *seek_entry (int state_from, int state_to)\n"
"{\n"
"  transfer_entry *entry;\n"
"  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)\n"
"  {\n"
"    entry = &transfer_table[state_from][i];\n"
"    if (entry->state == state_to) \n"
"    {\n"
"      break;\n"
"    }\n"
"  }\n"
"  return entry;\n"
"}\n"
"\n"
"void init_table (void)\n"
"{\n"
"  init_transfer();\n"
"\n"
"  add_transfer(TK_INIT, TK_IDENTIFIER_BEGIN, false, false, CHAR_CLASS_IDENTIFIER_BEGIN, init_token);\n"
"  add_transfer(TK_INIT, TK_INIT, false, false, CHAR_CLASS_SPACES, skip_token);\n"
"  add_transfer(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, true, false, CHAR_CLASS_SPACES, accept_token);\n"
"  add_transfer(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_BEGIN, false, false, CHAR_CLASS_IDENTIFIER_PART, append_token);\n"
"\n"
"}\n"
"\n"
"void check_init_table (void)\n"
"{\n"
"  void init_table(void);\n"
"  bool can_transfer(transfer_entry *entry, int character);\n"
"  init_table ();\n"
"\n"
"  /* TK_INIT -> TK_IDENTIFIER_BEGIN */\n"
"  for (int i=0;i<strlen (CHAR_CLASS_IDENTIFIER_BEGIN);++i) \n"
"  {\n"
"    int letter = CHAR_CLASS_IDENTIFIER_BEGIN[i];\n"
"    assert (can_transfer (seek_entry(TK_INIT, TK_IDENTIFIER_BEGIN), letter));\n"
"\n"
"  }\n"
"\n"
"  /* check TK_INIT -> TK_INIT */ \n"
"  for (int i=0;i<strlen (CHAR_CLASS_SPACES); ++i)\n"
"  {\n"
"    int space = CHAR_CLASS_SPACES[i];\n"
"    assert (can_transfer (seek_entry(TK_INIT, TK_INIT), space));\n"
"  }\n"
"\n"
"  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_END */\n"
"  for (int i=0;i<strlen (CHAR_CLASS_SPACES); ++i)\n"
"  {\n"
"    int letter = CHAR_CLASS_SPACES[i]; \n"
"    assert (can_transfer (seek_entry(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END), letter));\n"
"  }\n"
"\n"
"  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_BEGIN */\n"
"  for (int i=0;i<strlen (CHAR_CLASS_IDENTIFIER_PART); ++i)\n"
"  {\n"
"    int letter =CHAR_CLASS_IDENTIFIER_PART[i]; \n"
"    assert (can_transfer (seek_entry(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_BEGIN), letter));\n"
"  }\n"
"\n"
"\n"
"}\n"
"\n"
"\n"
"typedef enum char_operation\n"
"{\n"
"  CHAR_NEXT,\n"
"  CHAR_CURR,\n"
"  CHAR_PREV,\n"
"\n"
"  CHAR_GET,\n"
"  CHAR_PUT\n"
"\n"
"} char_operation;\n"
"\n"
"int get_next_token(token *tk, char_buffer *buffer)\n"
"{\n"
"  tokenizer_state current_state = TK_INIT;\n"
"  transfer_entry *current_entry;\n"
"  int current_char;\n"
"  bool is_accepted=false;\n"
"  tokenizer_state next_state;\n"
"  if (peek_char (buffer) == EOF)\n"
"  {\n"
"    return EOF;\n"
"  }\n"
"\n"
"  while ((current_char = peek_char (buffer)) != EOF )\n"
"  {\n"
"    next_state = do_transfer(current_state, current_char, &current_entry);\n"
"    if (next_state == TK_NULL)\n"
"    {\n"
"      return E_UNEXPECTED_CHAR;\n"
"    }\n"
"    is_accepted = current_entry->is_accepted;\n"
"    current_entry->tkz_act (tk, next_state, buffer);\n"
"    current_state = next_state;\n"
"    if (is_accepted)\n"
"      break;\n"
"  }\n"
"\n"
"  if (is_accepted) {\n"
"    return 0;\n"
"  }\n"
"\n"
"  return E_PREMATURE_END;\n"
"\n"
"}\n"
"\n"
"void check_all(void)\n"
"{\n"
"  void check_char_buffer(void);\n"
"  check_char_buffer();\n"
"  \n"
"  check_init_table();\n"
"}\n"
"\n"
"\n"
"int main(int ac,char**av){ \n"
"  check_all();\n"
"\n"
"  if (ac != 2)\n"
"  {\n"
"    goto error;\n"
"  }\n"
"\n"
"\n"
"  FILE *input;\n"
"  token tk;\n"
"  int r=0;\n"
"  char *token_string;\n"
"  char_buffer buffer;\n"
"\n"
"  if (init_char_buffer (&buffer, av[1])<0)\n"
"  {\n"
"    perror (av[0]);\n"
"    goto error;\n"
"  }\n"
"\n"
"  init_table();\n"
"  while ((r = get_next_token(&tk, &buffer)) != EOF)\n"
"  {\n"
"    switch (r)\n"
"    {\n"
"      case 0:\n"
"        token_string = format_token (&tk);\n"
"        puts(token_string);\n"
"        break;\n"
"\n"
"      default:\n"
"      case 1:\n"
"        goto error;\n"
"    }\n"
"  }\n"
"\n"
"  return 0;\n"
"\n"
"error:\n"
"  exit(1);\n"
"}\n"
"\n"
"\n"
"\n"
;
