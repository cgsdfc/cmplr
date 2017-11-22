int int_function(int int_arg_1)
{
  int local_int_1=1;
  char local_char_1='a';

  local_int_1=2;
  local_char_1='b';
  return (local_int_1 + 1);
}

int int_function_noargs()
{
  int local_int_2;
  int local_sum;

  local_sum=0;
  for (local_int_2=1; local_int_2 < 100; 
      local_int_2 = local_int_2 + 1)
    local_sum+=local_int_2;
  return (local_sum);
}

