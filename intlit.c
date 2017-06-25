#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>


int main(int ac,char**av){ 
  if (ac != 2)
  {
    puts("Usage : intlit N");
    exit(0);
  }

  int limit=atoi(av[1]);
  if (limit <= 0)
  {
    puts("limit <= 0");
    exit(1);
  }

  char *sub[]={"u","U","l","L","ul","lu","UL","LU","Lu","Ul","uL",""};
  int len=sizeof sub / sizeof sub[0];
  char *fmt[]={"%d%s\n", "0%o%s\n", "0x%x%s\n"};


  for (int i=0;i<limit;++i)
  {
    for (int j=0;j<3;j++)
    {
      for (int k=0;k<len;++k)
      {
        printf(fmt[j], i,sub[k]);
      }
    }
  }
 
}
