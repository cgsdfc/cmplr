int main() {
  int * a=malloc(sizeof (int) * 2 * 3);
  a[0][1]=1;
  printf("`%d'\n", a[0][1]);
}

