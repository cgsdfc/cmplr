int factoral(int N) {
  if (N < 2) return(1);
  return(N * factoral(N-1));
}

void main() {
  printf("factoral 16 ", factoral(16));
}
