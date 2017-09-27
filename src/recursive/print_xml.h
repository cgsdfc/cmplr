#ifndef RECURSIVE_PRINT_XML_H
#define RECURSIVE_PRINT_XML_H
#include <stdio.h>
struct node_base;
typedef struct xml_printer
{
  struct node_base *node;
  FILE *file;
  size_t indent;
  const char **tag_array;
} xml_printer;
void init_xml_printer (struct xml_printer *, struct node_base *, FILE *,
		       const char **);
void print_xml (struct xml_printer *, const char *);


#endif // RECURSIVE_PRINT_XML_H
