#ifndef RECURSIVE_PRINT_XML_H
#define RECURSIVE_PRINT_XML_H
#include <stdio.h>
struct node_base;
typedef struct xml_printer
{
  const char * filename;
  FILE *file;
  size_t indent;
  const char **tag_array;
} xml_printer;
void init_xml_printer (struct xml_printer *, FILE *,
    char *, const char **);
void print_xml (struct xml_printer *, struct node_base*);


#endif // RECURSIVE_PRINT_XML_H
