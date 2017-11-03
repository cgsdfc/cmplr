#include "abstract_scanner.h"


void utillib_abstract_scanner_init(struct utillib_abstract_scanner *self,
    void * scan, struct utillib_scanner_operations * op)
{
  self->scan=scan;
  self->op=op;
}

