void int_array_do_sth()
{
  int tmp;
  printf("Swaps int_array_1 and int_array_2");
  for (int_1=0; int_1 < const_int_1; int_1 = int_1 + 1) {
    tmp=int_array_1[int_1];
    int_array_1[int_1]=int_array_2[int_1];
    int_array_2[int_1]=tmp;
  }
}

void swap_sth()
{
  int tmp;
  printf("Swaps int_1 and int_2");
  tmp=int_1;
  int_1=int_2;
  int_2=tmp;
}


