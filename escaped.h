#ifndef ESCAPED_H
#define ESCAPED_H 1
#include "transfer.h"
#define N_ES_ROWS (ES_END - ES_INIT+1)
#define N_ES_COLS (ES_NULL - ES_INIT+1)


typedef enum escape_state
{

  ES_INIT=0,
  /* the '\\' is read */
  ES_BEGIN,  

  /* 'x' is read */
  ES_HEX_BEGIN,

  /* 2 hex digs are read */
  ES_HEX_END, 

  /* one oct dig is read */
  ES_OCT_BEGIN,

  /* 2 oct dig is read */
  ES_OCT_END, 
  
  /* '0' is read */
  ES_ZERO,

  /* skip quotes */
  ES_SKIP,

  /* 1 letter or 2 hex digs or 3 oct digs are read */
  ES_END,


  ES_NULL,
 
} escape_state;

void init_escaped(void);
void fini_escaped(void);

void check_escaped(void);
int eval_string(char *src, char *dst);
extern const char escaped_tab[];

#endif

