/* char_buffer.h */
#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H 1
#include <stdio.h>
#define CHAR_BUFFER_MAX_LINE BUFSIZ


typedef struct position
{
  int lineno;
  int column;

} position;

struct position;
typedef struct char_buffer 
{
  FILE *file;
  char *buf ;
  struct position pos;
  int index;
  int end;
  int *limits;
  int lines;

} char_buffer;

int init_char_buffer (char_buffer *buffer, char *);
int peek_char (char_buffer *);
int put_char (char_buffer *);
int get_char (char_buffer *);
int prev_char (char_buffer*);
int next_char (char_buffer*);
char *peek_line (char_buffer*,int);
int get_lineno(char_buffer*);

#endif

