#ifndef PASCAL_SCANNER_H
#define PASCAL_SCANNER_H
#include <utillib/scanner.h>
#include <utillib/string.h>

extern struct utillib_scanner_op const parser_scanner_op;

struct pascal_scanner {
  struct utillib_char_scanner char_scanner;
  struct utillib_string char_buffer;
  size_t code;
};

size_t pascal_scanner_lookahead(struct pascal_scanner *self);
void pascal_scanner_shiftaway(struct pascal_scanner *self);
void *pascal_scanner_getsymbol(struct pascal_scanner *self);

#endif // PASCAL_SCANNER_H
