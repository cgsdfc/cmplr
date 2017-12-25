int factoral(int N) {
  if (N < 2) return(1);
  return(N * factoral(N-1));
}

void main() {
  const int arg=-1;
  printf("factoral 10 ", factoral(arg));
}
