#ifndef PREP_ARG_H
#define PREP_ARG_H

typedef struct prep_arg_t {
  FILE *arg_file;
  char const *arg_filename;
  utillib_vector arg_incl_paths;
  utillib_vector arg_defs;
} prep_arg_t;

void prep_arg_init(prep_arg_t *, int, char **);
#endif // PREP_ARG_H
