#include <stdio.h>
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "tokenizer.h"


int main(int ac,char**av){ 
  char a='?';
  printf ("char a = %c\n", a);
  a='\?';
  printf ("char a = %c\n", a);
  a='"';
  printf ("char a = %c\n", a);
  short b='\xFF';
  printf ("chbr b = %x\n", b);
  char *bb="'";
  char *bbb="\'";
  assert (strcmp (bb, bbb)==0);

  char _='
    a';

}
