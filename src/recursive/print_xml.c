#include "print_xml.h"
#include "node_base.h"
#include <stdarg.h>
#include <stdio.h>
#define XML_PADDING_STR " "
#define XML_NEWLINE_STR "\n"

static void print_xml_impl(void *, void *);
static void print_xml_ternary_impl(xml_printer *p, ternary_node *ternary,
                                   const char *tag, const char *first_tag,
                                   const char *second_tag,
                                   const char *third_tag);

void init_xml_printer(xml_printer *p, FILE *file, char *filename,
                      const char **tag_array) {
  p->indent = 0;
  p->file = file;
  p->tag_array = tag_array;
  p->filename = filename;
}

static void print_xml_padding(xml_printer *p) {
  size_t indent = p->indent;
  while (indent--) {
    fputs(XML_PADDING_STR, p->file);
  }
}

static void print_xml_newline(xml_printer *p) {
  fputs(XML_NEWLINE_STR, p->file);
}

static void print_xml_indent(xml_printer *p) {
  print_xml_padding(p);
  ++p->indent;
}

static void print_xml_unindent(xml_printer *p) {
  --p->indent;
  print_xml_padding(p);
}

static const char *print_xml_access_tag(xml_printer *p, size_t index) {
  return p->tag_array[index];
}

static void print_xml_open_tag_impl(xml_printer *p, const char *tag, va_list ap,
                                    bool strict) {
  const char *key, *value;
  print_xml_indent(p);
  fprintf(p->file, "<%s", tag);
  for (key = va_arg(ap, const char *), value = va_arg(ap, const char *);
       key != NULL || value != NULL;
       key = va_arg(ap, const char *), value = va_arg(ap, const char *)) {
    fprintf(p->file, " %s=\"%s\"", key, value);
  }
  if (strict)
    fputs(">", p->file);
}

static void print_xml_open_tag_attri(xml_printer *p, const char *tag, ...) {
  // <tag attri="val">
  va_list ap;
  va_start(ap, tag);
  print_xml_open_tag_impl(p, tag, ap, true /* strict */);
  print_xml_newline(p);
  va_end(ap);
}

// adaptor
static void print_xml_open_tag(xml_printer *p, const char *tag) {
  print_xml_open_tag_attri(p, tag, NULL, NULL);
}

static void print_xml_close_tag_impl(xml_printer *p, const char *tag,
                                     bool strict) {
  if (strict) {
    print_xml_unindent(p);
    fprintf(p->file, "</%s>", tag);
  } else {
    fputs("/>", p->file);
    print_xml_unindent(p);
  }
  print_xml_newline(p);
}

static void print_xml_close_tag(xml_printer *p, const char *tag) {
  print_xml_close_tag_impl(p, tag, true /* strict */);
}

static void print_xml_unstrict_tag(xml_printer *p, const char *tag, ...) {
  // <tag attri="val"/>
  va_list ap;
  va_start(ap, tag);
  print_xml_open_tag_impl(p, tag, ap, false /* unstrict */);
  print_xml_close_tag_impl(p, tag, false /* unstrict */);
  va_end(ap);
}

static void print_xml_pair_of_tag(xml_printer *p, const char *tag,
                                  node_base *node) {
  // <tag> ... </tag>
  print_xml_open_tag(p, tag);
  print_xml_impl(p, node);
  print_xml_close_tag(p, tag);
}

// ====================================================================== //
// print_xml_nullary
// ====================================================================== //
static void print_xml_nullary(void *printer, void *node) {
  print_xml_unstrict_tag(printer, "nullary", NULL, NULL);
}

// ====================================================================== //
// print_xml_terminal
// ====================================================================== //
static void print_xml_terminal_impl(xml_printer *p, terminal_node *terminal) {
  Token *tk = terminal->token;
  print_xml_unstrict_tag(p, "terminal", "token_type",
                         (char *)print_xml_access_tag(p, TOKEN_TYPE(tk)),
                         "string", TOKEN_STRING(tk), NULL, NULL);
}

static void print_xml_terminal(void *printer, void *node) {
  print_xml_terminal_impl(printer, node);
}

// ====================================================================== //
// print_xml_unary
// ====================================================================== //
static void print_xml_unary_impl(xml_printer *p, unary_node *unary,
                                 const char *tag, const char *operand_tag) {
  print_xml_open_tag(p, tag);
  print_xml_pair_of_tag(p, operand_tag, unary->operand);
  print_xml_close_tag(p, tag);
}

static void print_xml_unary_expr(xml_printer *p, unary_node *unary) {
  print_xml_unary_impl(p, unary, print_xml_access_tag(p, unary->op), "operand");
}

static void print_xml_if_stmt(xml_printer *p, unary_node *if_node) {
  print_xml_ternary_impl(p, IF_NODE_BODY(if_node), "if", "condition", "then",
                         "else");
}

static void print_xml_goto_stmt(xml_printer *p, unary_node *goto_node) {
  print_xml_unary_impl(p, goto_node, "goto", "label");
}

static void print_xml_default_stmt(xml_printer *p, unary_node *default_node) {
  print_xml_unary_impl(p, default_node, "default", "statement");
}

static void print_xml_return_stmt(xml_printer *p, node_base *return_node,
                                  bool has_expr) {
  if (has_expr) {
    // if has_expr, return_node is a unary node
    print_xml_unary_impl(p, (unary_node *)return_node, "return", "expression");
  } else {
    // else, return node is a terminal_node
    print_xml_terminal_impl(p, (terminal_node *)return_node);
  }
}

static void print_xml_unary(void *p, void *node) {
  unary_node *unary = node;
  switch (UNARY_NODE_OP(unary)) {
  case TKT_KW_IF:
    print_xml_if_stmt(p, unary);
    break;
  case TKT_KW_GOTO:
    print_xml_goto_stmt(p, unary);
    break;
  case TKT_KW_DEFAULT:
    print_xml_goto_stmt(p, unary);
    break;
  case TKT_KW_RETURN:
    print_xml_return_stmt(p, node, true);
    break;
  default:
    print_xml_unary_expr(p, unary);
    break;
  }
}

// ====================================================================== //
// print_xml_binary
// ====================================================================== //
static void print_xml_binary_impl(xml_printer *p, binary_node *binary,
                                  const char *tag, const char *lhs_tag,
                                  const char *rhs_tag) {
  print_xml_open_tag(p, tag);
  print_xml_pair_of_tag(p, lhs_tag, binary->lhs);
  print_xml_pair_of_tag(p, rhs_tag, binary->rhs);
  print_xml_close_tag(p, tag);
}

static void print_xml_switch_stmt(xml_printer *p, binary_node *switch_node) {
  print_xml_binary_impl(p, switch_node, "switch", "expression", "statement");
}

static void print_xml_binary_expr(xml_printer *p, binary_node *binary) {
  print_xml_binary_impl(p, binary,
                        print_xml_access_tag(p, BINARY_NODE_OP(binary)),
                        "left-operand", "right-operand");
}

static void print_xml_for_stmt(xml_printer *p, binary_node *for_node) {
  // cannot use print_xml_binary_impl here
  // since the behaviour of print_xml_ternary_impl
  // need to be tuned.
  print_xml_open_tag(p, "for");
  print_xml_ternary_impl(p, FOR_NODE_HEAD(for_node), "head", "initial",
                         "condition", "step");
  print_xml_pair_of_tag(p, "body", FOR_NODE_BODY(for_node));
  print_xml_close_tag(p, "for");
}

static void print_xml_do_while_stmt(xml_printer *p,
                                    binary_node *do_while_node) {
  print_xml_binary_impl(p, do_while_node, "do-while", "stmt", "expr");
}

static void print_xml_while_stmt(xml_printer *p, binary_node *while_node) {
  print_xml_binary_impl(p, while_node, "while", "expr", "stmt");
}

static void print_xml_compound_stmt(xml_printer *p, binary_node *compound) {
  print_xml_binary_impl(p, compound, "compound", "decl_list", "stmt_list");
}

static void print_xml_case_stmt(xml_printer *p, binary_node *case_node) {
  print_xml_binary_impl(p, case_node, "case", "const-expr", "stmt");
}

static void print_xml_binary(void *printer, void *node) {
  binary_node *binary = node;
  switch (BINARY_NODE_OP(binary)) {
  case TKT_KW_CASE:
    print_xml_case_stmt(printer, binary);
    break;
  case TKT_KW_FOR:
    print_xml_for_stmt(printer, binary);
    break;
  case TKT_KW_WHILE:
    print_xml_while_stmt(printer, binary);
    break;
  case TKT_KW_DO:
    print_xml_do_while_stmt(printer, binary);
    break;
  case TKT_STMT_COMPOUND:
    print_xml_compound_stmt(printer, binary);
    break;
  default:
    print_xml_binary_expr(printer, binary);
    break;
  }
}

// =============================================================== //
// print_xml_ternary
// =============================================================== //
static void print_xml_ternary_impl(xml_printer *p, ternary_node *ternary,
                                   const char *tag, const char *first_tag,
                                   const char *second_tag,
                                   const char *third_tag) {
  print_xml_open_tag(p, tag);
  print_xml_pair_of_tag(p, first_tag, ternary->first);
  print_xml_pair_of_tag(p, second_tag, ternary->second);
  print_xml_pair_of_tag(p, third_tag, ternary->third);
  print_xml_close_tag(p, tag);
}

static void print_xml_ternary(void *printer, void *node) {
  print_xml_ternary_impl(printer, node, "ternary", "first", "second", "third");
}

// =============================================================== //
// print_xml_vector
// ============================================================== //
static void print_xml_vector_impl(xml_printer *p, vector_node *v) {
  size_t size = vector_node_size(v);
  char size_buf[20];
  sprintf(size_buf, "%lu", size);
  print_xml_open_tag_attri(p, "vector", "size", size_buf, NULL, NULL);
  for (int i = 0; i < size; ++i) {
    sprintf(size_buf, "%d", i);
    print_xml_pair_of_tag(p, size_buf, vector_node_at(v, i));
  }
  print_xml_close_tag(p, "vector");
}

static void print_xml_vector(void *printer, void *v) {
  print_xml_vector_impl(printer, v);
}

// =============================================================== //
// print_xml
// =============================================================== //
static vfunction *print_xml_factory(void *data, node_tag tag) {
  static vfunction *print_xml_table[] =
      {[NODE_TAG_NULLARY] = print_xml_nullary,
       [NODE_TAG_TERMINAL] = print_xml_terminal,
       [NODE_TAG_UNARY] = print_xml_unary,
       [NODE_TAG_BINARY] = print_xml_binary,
       [NODE_TAG_TERNARY] = print_xml_ternary,
       [NODE_TAG_VECTOR] = print_xml_vector,
       NULL};
  vfunction *visit = print_xml_table[tag];
  assert(visit);
  return visit;
}

static void print_xml_impl(void *printer, void *node) {
  visit_node(printer, node, print_xml_factory);
}

static void print_xml_comment(xml_printer *p) {
  const char *str = "<!-- source code %s -->" XML_NEWLINE_STR;
  fprintf(p->file, str, p->filename);
}

void print_xml(struct xml_printer *p, node_base *node) {
  print_xml_comment(p);
  print_xml_impl(p, node);
}
