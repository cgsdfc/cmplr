#ifndef UTILLIB_ABSTRACT_SCANNER_H
#define UTILLIB_ABSTRACT_SCANNER_H

#define utillib_abstract_scanner_get_current_symbol(self) ((self)->op((self)->scan))
#define utillib_abstract_scanner_get_next_symbol(self) ((self)->op((self)->scan))

int utillib_abstract_scanner_get_next_symbol(struct utillib_abstract_scanner *self);
struct utillib_scanner_operations {
  int (*get_current_symbol) (void *scan);
  int (*get_next_symbol) (void *scan);
};

struct utillib_abstract_scanner {
  void *scan;
  struct utillib_scanner_operations *op;
};

void utillib_abstract_scanner_init(struct utillib_abstract_scanner *self,
    void * scan, struct utillib_scanner_operations * op);
#endif // UTILLIB_ABSTRACT_SCANNER_H
