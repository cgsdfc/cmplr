void void_function_noargs()
{
  return;
}

void void_function_noargs_2()
{
  int local_int_1;
  scanf(local_int_1);
  if (local_int_1 == 1) {
    void_function('a', local_int_1, 'b');
  } else if (local_int_1 == 2) {
    int local_int_2=int_function_noargs();
    printf("Calls int_function_noargs returns ");
    printf(local_int_2, "");
  } else {
    int local_int_2;
    for (local_int_2=2; local_int_2 < 3; 
       local_int_2=local_int_2+1) {
      printf("void_function_noargs_2 does loop ", local_int_2);
    }
  }
}
