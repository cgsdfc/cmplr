const int Length=10;
int Array[10];

void ArrayFill() 
{
  int i;
  for (i=0; i<Length; i=i+1) {
    Array[i]=i;
  }
}

void ArrayPrint()
{
  int i;
  for (i=0; i<Length; i=i+1) {
    printf("#", i);
    printf(" = ", Array[i]);
    printf(" ");
  }
}

void main()
{
  ArrayFill();
  ArrayPrint();
}
