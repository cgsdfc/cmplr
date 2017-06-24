#include <stdio.h>
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "tokenizer.h"

int a[5][5];

int func0(int a[][4], int rows, int cols)
{
  for (int i=0;i<rows;++i)
  {
    for(int j=0;j<cols;++j)
    {
      printf("%d..\n", a[i][j]);
    }
  }
}

/* segment fault */
int func1(int **a, int rows, int cols)
{
  for (int i=0;i<rows;++i)
  {
    for(int j=0;j<cols;++j)
    {
      printf("%d..\n", a[i][j]);
    }
  }
}

/* segment fault */

int func(int *a[], int rows, int cols)
{
  for (int i=0;i<rows;++i)
  {
    for(int j=0;j<cols;++j)
    {
      printf("%d..\n", a[i][j]);
    }
  }
}


int main(int ac,char**av){ 

  int i=0111111;
  int j=07lu;
  int k=02ul;
  int l=1UL;
  int a=4u;
  int n=0ul;
  int e=0;
 
  char c='\1';

  char *hhh="\xff";
  printf("%s", hhh);
  puts("compiled!");

}
