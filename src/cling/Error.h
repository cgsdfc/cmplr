#ifndef CLING_ERROR_HXX
#define CLING_ERROR_HXX
#include <stdio.h>
struct Scanner;
enum {
  CL_EEXPECT = 1,
  CL_EUNEXPECTED,
  CL_EREDEFINED,
  CL_EINCTYPE,
  CL_EUNDEFINED,
  CL_ENOTLVALUE,
  CL_EINCARG,
  CL_EARGCUNMAT,
  CL_EDUPCASE,
  CL_EBADCASE,
  CL_EINVEXPR,
  CL_EBADTOKEN,
};

struct Error {
  unsigned int row;
  unsigned int col;
  char const *context;
  Error(Scanner const *scanner, int context);
  virtual void print(FILE *file)=0;
  virtual ~Error()=0;
};

struct EList {
  std::vector<Error*> elist;
  void AddError(Error *error);
  ~EList();
}

struct RedefinedError: public Error {
  char *name;
  void print(FILE *file) override;
};

struct UndefinedError: public Error {
  char *name;
  void print(FILE *file) override;
};

#endif
