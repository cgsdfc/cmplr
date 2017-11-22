int recursive_function_1(int int_arg_1)
{
  if (int_arg_1 < 2)
    return 1;
  return (int_arg_1 * recursive_function_1(int_arg_1-1))
}

void recursive_function_2(char A, char B,char C,int n)
{
    if(n==1) {
      printf("Move disk ", n);
      printf(" from ", A);
      printf(" to ",C);
    } else {
      recursive_function_2(A,C,B, n-1);
      printf("Move disk ", n);
      printf("from ", A,);
      printf("to ", C);
      recursive_function_2(B,A,C,n-1);
    }
}

void recursive_function_3(int int_arg_1)
{
  const int cnt=4;

  if (int_arg_1 == 0) {
    printf("End of recursive_function_3");
    return;
  }
  if (int_arg_1 == 1) {
    int i;
    for (i=0; i<cnt; i= i+1) {
      recursive_function_3(i);
    }
    return;
  }
  if (int_arg_1 == 2) {
    int i,j;
    for (i=0; i<cnt; i=i+1)
      for (j=0; j<cnt; j=j+1)
        recursive_function_3(i);
    return ;
  }
  if (int_arg_1 == 3) {
    int i,j, k;
    for (i=0; i<cnt; i=i+1)
      for (j=0; j<cnt; j=j+1)
        for (k=0; k<cnt; k= k+1)
          recursive_function_3(i);
    return ;
  }
}
