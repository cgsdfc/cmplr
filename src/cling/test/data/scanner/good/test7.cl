void main()
{
  int select;

  printf("const_int_decl=", const_int_decl);
  printf("const_int_1=", const_int_1);

  printf("const_char_add=", const_char_add);
  printf("const_char_number=", const_char_number);
  print_usage();

  for (int_1=0; int_1 < 1; int_1 = int_1 - 1 ) {
    printf("(^_^)");
    scanf(select);
    switch (select) {
    case 0:
      printf("int_function takes one int. please input that:");
      scanf(int_1);
      printf("int_function returns ", int_function(int_1));
    case 1:
      printf("int_function_noargs returns ", int_function_noargs());
    case 2:
      printf("void_function takes one char. please input that:");
      scanf(char_1);
      void_function(char_1, const_int_zero, const_char_add);
    case 3:
      printf("void_function_noargs does nothing.");
      void_function_noargs();
    case 4:
      printf("void_function_noargs_2 takes one int. please input that:");
      scanf(int_1);
      void_function_noargs_2(int_1);
    case 5:
      printf("recursive_function_1 takes one int. please input that:");
      scanf(int_1);
      recursive_function_1(int_1);
    case 6:
      printf("recursive_function_3 takes one int. please input that:");
      scanf(int_1);
      recursive_function_3(int_1);
    case 7:
      printf("recursive_function_2 takes one int. please input that:");
      scanf(int_1);
      recursive_function_2(const_char_add, const_char_div, const_char_mul, int_1);
    case 8:
      int_array_do_sth();
    case 9:
      swap_sth();
    case 'q':
      return (0);
    case 'h':
      print_usage();
    default:
      printf("Not understood. type `h'.");
    }
  }
}


