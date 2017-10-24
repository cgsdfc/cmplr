#ifndef CHAR_CLASS_H
#define CHAR_CLASS_H 1

struct dfa_state;
int alloc_char_class(const char *chcl);
int cond_char_class(struct dfa_state *, int);
void destroy_char_class(void);

#endif
