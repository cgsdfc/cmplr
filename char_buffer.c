#include "char_buffer.h"
#include <string.h>
#include<stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

static int count_chars(char *file)
{
  struct stat statbuf;
  if (stat(file, &statbuf)!=0)
  {
    return -1;
  }
  return statbuf.st_size;

}

static int count_lines(char *file)
{
  FILE *f;
  int ch;

  if ((f=fopen(file,"r"))==NULL)
  {
    return -1;
  }
  for (int line=0, ch=fgetc(f);;ch=fgetc(f))
  {
    if (ch==EOF) {
      fclose (f);
      return line;
    }
    if (ch=='\n')
      line++;
  }
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
int init_char_buffer (char_buffer *buffer, char *file)
{
  FILE *f;
  void *buf,*limits;
  char *errmsg="";
  int lines=0;
  int chars=0;
  char line[BUFSIZ];
  int i=0;

  do {
    if ((f=fopen (file,"r"))==NULL)
    {
      errmsg="fopen";
      break;
    }

    if ((chars=count_chars(file))<0)
    {
      errmsg="count_chars";
      break;
    }

    if ((lines=count_lines(file))<0)
    {
      errmsg="count_lines";
      break;
    }


    if ((buf=malloc(sizeof (char) * ( chars + 5)))==NULL)
    {
      errmsg="malloc";
      break;
    }

    if ((limits=malloc(sizeof (int) * (1 + lines)))==NULL)
    {
      errmsg="malloc";
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

  perror (errmsg);
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

  printf("limit = %d, offset = %d\n",
      limit, offset);

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
  char *file="./test/init.c";

  r=init_char_buffer (&buffer, file);
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
