#include <stdio.h>
#include <stdarg.h>
#include "node_base.h"
#include "print_xml.h"
#define XML_PADDING_STR " "
#define XML_NEWLINE_STR "\n"
#define XML_SET_NODE(P, N) ((P)->node = (N))
#define XML_GET_NODE(P) ((P)->node)

static void print_xml_impl (xml_printer * p);
void
init_xml_printer (xml_printer * p, node_base * node, FILE * file,
		  const char **tag_array)
{
  p->node = node;
  p->indent = 0;
  p->file = file;
  p->tag_array = tag_array;
}

static void
print_xml_padding (xml_printer * p)
{
  size_t indent = p->indent;
  while (indent--)
    {
      fputs (XML_PADDING_STR, p->file);
    }
}

static void
print_xml_newline (xml_printer * p)
{
  fputs (XML_NEWLINE_STR, p->file);
}

static void
print_xml_indent (xml_printer * p)
{
  print_xml_padding (p);
  ++p->indent;
}

static void
print_xml_unindent (xml_printer * p)
{
  --p->indent;
  print_xml_padding (p);
}

static const char *
print_xml_access_tag (xml_printer * p, size_t index)
{
  return p->tag_array[index];
}

static void
print_xml_open_tag_impl (xml_printer * p, const char *tag, va_list ap,
			 bool strict)
{
  const char *key, *value;
  print_xml_indent (p);
  fprintf (p->file, "<%s", tag);
  for (key = va_arg (ap, const char *),
       value = va_arg (ap, const char *);
       key != NULL || value != NULL;
       key = va_arg (ap, const char *), value = va_arg (ap, const char *))
    {
      fprintf (p->file, " %s=\"%s\"", key, value);
    }
  if (strict)
    fputs (">", p->file);
}

static void
print_xml_open_tag_attri (xml_printer * p, const char *tag, ...)
{
  // <tag attri="val">
  va_list ap;
  va_start (ap, tag);
  print_xml_open_tag_impl (p, tag, ap, true /* strict */ );
  print_xml_newline (p);
  va_end (ap);
}

// adaptor
static void
print_xml_open_tag (xml_printer * p, const char *tag)
{
  print_xml_open_tag_attri (p, tag, NULL, NULL);
}

static void
print_xml_close_tag_impl (xml_printer * p, const char *tag, bool strict)
{
  if (strict)
    {
      print_xml_unindent (p);
      fprintf (p->file, "</%s>", tag);
    }
  else
    {
      fputs ("/>", p->file);
      print_xml_unindent (p);
    }
  print_xml_newline (p);
}

static void
print_xml_close_tag (xml_printer * p, const char *tag)
{
  print_xml_close_tag_impl (p, tag, true /* strict */ );
}

static void
print_xml_unstrict_tag (xml_printer * p, const char *tag, ...)
{
  // <tag attri="val"/>
  va_list ap;
  va_start (ap, tag);
  print_xml_open_tag_impl (p, tag, ap, false /* unstrict */ );
  print_xml_close_tag_impl (p, tag, false /* unstrict */ );
  va_end (ap);
}

static void
print_xml_pair_of_tag (xml_printer * p, const char *tag, node_base * node)
{
  // <tag> ... </tag>
  print_xml_open_tag (p, tag);
  XML_SET_NODE (p, node);
  print_xml_impl (p);
  print_xml_close_tag (p, tag);
}


static void
print_xml_terminal (xml_printer * p)
{
  terminal_node *t = (terminal_node *) XML_GET_NODE (p);
  Token *tk = t->token;
  print_xml_unstrict_tag (p, "terminal",
			  "token_type", print_xml_access_tag (p,
							      TOKEN_TYPE
							      (tk)),
			  "string", TOKEN_STRING (tk), NULL, NULL);
}

static void
print_xml_unary (xml_printer * p)
{
  unary_node *unary = (unary_node *) XML_GET_NODE (p);
  print_xml_indent (p);
  print_xml_open_tag_attri (p, "unary",
			    "operator", print_xml_access_tag (p, unary->op),
			    NULL, NULL);
  // operand;
  print_xml_pair_of_tag (p, "operand", unary->operand);
  print_xml_close_tag (p, "unary");
}

static void
print_xml_binary (xml_printer * p)
{
  binary_node *binary = (binary_node *) XML_GET_NODE (p);
  print_xml_open_tag_attri (p, "binary",
			    "operator", print_xml_access_tag (p, binary->op),
			    NULL, NULL);
  // lhs
  print_xml_pair_of_tag (p, "lhs", binary->lhs);
  print_xml_pair_of_tag (p, "rhs", binary->rhs);
  // rhs 
  print_xml_close_tag (p, "binary");
}

static void
print_xml_vector (xml_printer * p)
{
  vector_node *v = (vector_node *) XML_GET_NODE (p);
  size_t size = vector_node_size (v);
  char size_buf[20];
  sprintf (size_buf, "%lu", size);
  print_xml_open_tag_attri (p, "vector", "size", size_buf, NULL, NULL);
  for (int i = 0; i < size; ++i)
    {
      sprintf (size_buf, "%d", i);
      // elements
      // <0>...<0>
      // <1>...<1>
      // ...
      print_xml_pair_of_tag (p, size_buf, vector_node_at (v, i));
    }
  print_xml_close_tag (p, "vector");
}

static void
print_xml_ternary (xml_printer * p)
{
  ternary_node *ternary = (ternary_node *) XML_GET_NODE (p);
  print_xml_open_tag (p, "ternary");
  print_xml_pair_of_tag (p, "first", ternary->first);
  print_xml_pair_of_tag (p, "second", ternary->second);
  print_xml_pair_of_tag (p, "third", ternary->third);
  print_xml_close_tag (p, "ternary");
}

static void
print_xml_nullary(xml_printer *p)
{
  print_xml_unstrict_tag(p, "nullary",NULL,NULL);
}

static void
print_xml_impl (xml_printer * p)
{
  switch (NODE_TAG (p->node))
    {
    case NODE_TAG_TERMINAL:
      print_xml_terminal (p);
      break;
    case NODE_TAG_UNARY:
      print_xml_unary (p);
      break;
    case NODE_TAG_BINARY:
      print_xml_binary (p);
      break;
    case NODE_TAG_VECTOR:
      print_xml_vector (p);
      break;
    case NODE_TAG_TERNARY:
      print_xml_ternary (p);
      break;
    case NODE_TAG_NULLARY:
      print_xml_nullary (p);
      break;
    }
}

static void
print_xml_comment (xml_printer * p, const char *filename)
{
  fprintf (p->file, "<!-- source code %s -->" XML_NEWLINE_STR, filename);
}

void
print_xml (struct xml_printer *p, const char *filename)
{
  print_xml_comment (p, filename);
  print_xml_impl (p);
}
