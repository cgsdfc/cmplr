#ifndef UTILLIB_ABSTRACT_SCANNER_H
#define UTILLIB_ABSTRACT_SCANNER_H

struct utillib_scanner_op {
  size_t (* lookahead ) (void *);
  void (* shiftaway ) (void *);
};

#endif // UTILLIB_ABSTRACT_SCANNER_H
