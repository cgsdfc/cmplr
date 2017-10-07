#define A(v, a) [v]=a
#define B(a) A(a, #a)

int
main ()
{
  const char *a[] = {
    B (1)
  };
}
