int global_int;
char global_char;


void test_global() {
  scanf(global_int);
  printf(global_int);
  scanf(global_char);
  printf(global_char);
}

void main() {
  int i;
  for (i=0; i<=0; i=i-1) {
    printf("test_global ");
    test_global();
  }
}
