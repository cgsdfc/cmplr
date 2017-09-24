#include <stdbool.h>
#include "lexer/lexer.h"

// experiment recursive parser to parse
// simple expression like 
// expr := term ((+|-) term)*
// term := number ((*|/) number)*
// number := (integer|float)

typedef enum node_tag
{
  NODE_TAG_ADD,
  NODE_TAG_SUB,
  NODE_TAG_MUL,
  NODE_TAG_DIV,
  NODE_TAG_NUMBER,
  NODE_TAG_EXPR,
  NODE_TAG_TERM,
  NODE_TAG_INTEGER,
  NODE_TAG_FLOAT,
} node_tag;

typedef struct basic_node {
  node_tag tag;
};

typedef struct unary_node {
  node_tag tag;
  void *node;
} unary_node;

typedef struct binary_node {
  node_tag tag;
  unary_node lhs;
  unary_node rhs;
} binary_node;

typedef enum parse_result {
  PARSE_RESULT_SUCCESS,
  PARSE_RESULT_FAILED,
  PARSE_RESULT_EOFHIT,
} parse_result;

typedef struct parse_info {
  void *node;
  parse_result res;
} parse_info;
  
bool success(parse_info * info) 
{
  return info -> res == PARSE_RESULT_SUCCESS;
}

bool failed(parse_info * info)
{
  return info -> res == PARSE_RESULT_FAILED;
}

void set_success(parse_info * info)
{
  info->res = PARSE_RESULT_SUCCESS;
}

void set_failed(parse_info * info)
{
  info->res = PARSE_RESULT_FAILED;
}

void set_eofhit(parse_info * info)
{
  info->res = PARSE_RESULT_EOFHIT;
}

void set_node(parse_info * info, void *node)
{
  info->node = node;
}

bool check_eof(token_buffer * tokens)
{
  token * tk;
  return peek_token(tokens, &tk) == EOF;
}

unary_node *
make_unary_node(node_tag tag, token *tk)
{
  unary_node * node = malloc(sizeof *node);
  if (node) {
    node -> node=tk;
    node -> tag = tag;
  }
  return node;
}

binary_node *
make_binary_node(node_tag tag)
{
  binary_node * node = malloc(sizeof *node);
  if (node) {
    node->tag=tag;
  }
  return node;
}

void parse_success(token_buffer * tokens, parse_info * info, void * node)
{
  if(node) {
    token * tk;
    set_node(info, node);
    get_token(tokens, &tk);
    set_success(info);
  }
}

void
parse_number(token_buffer * tokens, parse_info * info)
{
  if (check_eof(tokens)) {
    set_eofhit(info);
    return;
  }
  token *tk;
  node_tag tag;
  peek_token(tokens, &tk);
  switch (TOKEN_TYPE(tk)) {
    case TKT_INT_CONST:
      tag = NODE_TAG_INTEGER;
      break;
    case TKT_FLOAT_CONST:
      tag = NODE_TAG_FLOAT;
      break;
    default:
      return;
  }
  parse_success(tokens, info, make_unary_node(tag, tk));
}

void parse_term_oper(token_buffer * tokens, parse_info * info)
{
  if (check_eof(tokens)) {
    set_eofhit(info);
    return;
  }
  token * tk;
  node_tag tag;
  peek_token(tokens, &tk);
  switch (TOKEN_TYPE(tk)) {
    case TKT_PLUS:
      tag = NODE_TAG_ADD;
      break;
    case TKT_MINUS:
      tag = NODE_TAG_SUB;
      break;
    default:
      return;
  }
  parse_success(tokens, info, make_binary_node(tag));
}

void parse_term(token_buffer * tokens, parse_info * info)
{
  set_failed(info), parse_number(tokens, info);
  if (!success(info)) {
    // error
    return;
  }
  unary_node * lhs = info->node;
  node_tag tag=NODE_TAG_NUMBER;
  // oper number *
  set_failed(info), parse_term_oper(tokens, info);
  while (success(info)) {
    binary_node * op = info->node;
    op->lhs.node=lhs;
    op->lhs.tag=tag;
    lhs=op;
    tag=NODE_TAG_TERM;
    set_failed(info), parse_number(tokens, info);
    if (failed(info)) {
      // error
    }
    unary_node * rhs = info->node;
    op->rhs.node=rhs; 
    op->rhs.tag=NODE_TAG_NUMBER;
    set_failed(info), parse_term_oper(tokens, info);
  }
  set_success(info);
  set_node(info, lhs);
}

parse_info parse(token_buffer * tokens) 
{
  parse_info info;
  parse_term(tokens, &info);
  return info;
}

void print_number(token *tk)
{
  printf("%s", TOKEN_STRING(tk));
}

void print_unary_node(unary_node * unary) 
{
  void print_binary_node(binary_node*);
  switch(unary->tag) {
    case NODE_TAG_TERM:
      print_binary_node(unary->node);
      break;
    case NODE_TAG_NUMBER:
      print_number(unary->node);
      break;
  }
}

void print_binary_node(binary_node * bin)
{
  node_tag tag=bin->tag;
  char op;
  switch(tag) {
    case NODE_TAG_ADD:
      op='+';
      break;
    case NODE_TAG_SUB:
      op='-';
      break;
  }
  printf("%c", op);
  putchar(' ');
  print_unary_node(&(bin->rhs));
  putchar(' ');
  print_unary_node(&(bin->lhs));
}

void print_node(void *node)
{
  print_binary_node(node);
}

int main(int argc, char **argv) 
{
  if (argc !=2)
  {
    fprintf(stderr, "Usage simple_expr <FILE>\n");
    exit(1);
  }
  token_buffer * tokens = create_lexer(argv[1]);
  if (!tokens) {
    fprintf(stderr, "simple_expr: create_lexer failed\n");
    exit (2);
  }
  token *tk;
  /* for (;peek_token(tokens, &tk)!=EOF; */
  /*     get_token(tokens, &tk)) { */
  /*   puts(format_token(tk)); */
  /* } */
  parse_info info = parse(tokens);
  print_node(info.node);
  return 0;
}
