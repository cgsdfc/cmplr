
int mod(int x, int y)
{
    int z;
    z = x - x/y*y;
    return(z);
}

int gcd(int a, int b)
{
    if (b==0) return(0);
    else if (mod(a,b)==0)
        return(b);
    else return(gcd(b, mod(a,b)));
}

void main()
{
  printf("gcd 160, 40 ", gcd(160, 40));
printf("gcd 180 20 ", gcd(180, 20));
}
