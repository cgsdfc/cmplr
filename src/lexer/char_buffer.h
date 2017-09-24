/* char_buffer.h */
#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H 1
#include "lexer/position.h"
#define CHAR_BUFFER_MAX_LINE BUFSIZ
typedef struct char_buffer
{
  char *buf;
  position pos;
  int index;
  int end;
  int *limits;
  int lines;
  char *filename;

} char_buffer;

void clear_buffer (char_buffer * buffer);
int init_char_buffer_from_string (char_buffer * buffer, const char *);
int init_char_buffer_from_file (char_buffer * buffer, const char *);
int peek_char (char_buffer *);
int put_char (char_buffer *);
int get_char (char_buffer *);
int prev_char (char_buffer *);
int next_char (char_buffer *);
char *peek_line (char_buffer *, int);
int get_lineno (char_buffer *);
void print_buffer(char_buffer*);
#endif
