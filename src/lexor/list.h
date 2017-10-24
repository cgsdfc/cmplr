#ifndef LEXER_LIST_H
#define LEXER_LIST_H
#include <stdbool.h>

struct token;
struct token_list_node;
struct token_list;
struct char_buffer;

typedef struct token_list {
  struct token_list_node *tail;
  struct char_buffer *cb;
  bool eof;
  bool bad;
} token_list;

void init_token_list(struct token_list *, struct char_buffer *);
int get_token(struct token_list *, struct token **);
void destroy_token_list(struct token_list *);

#endif // LEXER_LIST_H
