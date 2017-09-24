/* tokenizer.h */
#ifndef TOKENIZER_H
#define TOKENIZER_H 1

enum
{
  TKA_SKIP = 0,
  TKA_COLLECT_CHAR = 1,
  TKA_ALLOC_BUF,
  TKA_ACC_IDFR,
  TKA_ACC_PUNC,
  TKA_ACC_OPER,
  TKA_ACC_CHAR,
  TKA_ACC_INT,
  TKA_ACC_FLOAT,
  TKA_ACC_STRING,
  TKA_PUTBACK,
};

struct char_buffer;
struct token;
int get_next_token (struct char_buffer *,struct  token *);
int init_tokenizer (void);
int unbuffer_print_token_stream (struct char_buffer *);


#endif
