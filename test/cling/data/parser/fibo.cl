const int MAX=1000;

int fibo(int n)
{
  if (n<0) return(1);
  else if (n==0) return(0);
  else if (n==1) return(1);
  else if (n>MAX) return(2);
  else return(fibo(n-1)+fibo(n-2));
}

void test_fibo(int actual, int expect) {
  int result;
  result=fibo(actual);
  if (result != expect) {
    printf("result = ", result);
    printf("expect = ", expect);
    printf("Error");
  }
}

void main() 
{
  printf(fibo(0));
  printf(fibo(1));
  printf(fibo(2));
  printf(fibo(3));
  printf(fibo(4));
  printf(fibo(5));
  printf(fibo(6));
  printf(fibo(7));
  printf(fibo(8));
}

