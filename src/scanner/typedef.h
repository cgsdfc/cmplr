#ifndef SCANNER_TYPEDEF_H
#define SCANNER_TYPEDEF_H
struct scanner_base_t;

typedef int(scanner_match_func_t)(struct scanner_base_t *);
#endif // SCANNER_TYPEDEF_H
