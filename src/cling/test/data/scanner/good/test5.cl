void print_usage() 
{
  printf("Enter a number to select one function to run:");
  printf("  `0': int_function: takes one int arg and returns a local int variable");
  printf("  `1': int_function_noargs: takes no args and returns a somehow sum of something");
  printf("  `2': void_function: take in a char as command and displays the other args according to that command char");
  printf("                      if that char is 'a', it displays the second arg. if the char is 'b', it displays the third arg");
  printf("  `3': void_function_noargs: takes and returns nothing. the most boring function.");
  printf("  `4': void_function_noargs_2: scanfs an int and does different things according to the inputted int.");
  printf("  `5': recursive_function_1: scanfs an int and recursively computes the factual of it");
  printf("  `6': recursive_function_3: scanfs an int and does different recursions according to the input");
  printf("  `7': recursive_function_2: scanfs an int and plays HANOI game with you.");
  printf("  `8': int_array_do_sth: swaps the int_array_1 and int_array_2, element-wise");
  printf("  `9': swap_sth: swaps int_1 and int_2");
  printf("  `h': shows this menu argain.")
  printf("  `q': quits");
}

