int global_val;

int foo(int n){
	const int static_val = 0;
	int i;
	for(i = 0;i < 10;i = i + 1){
		n = n * i +  global_val + static_val;
	}
	global_val = n / 2;
	printf("static_val is %d",static_val);
	return (n);
}
void main()
{
	int a,b;
	a = 3;
	global_val = 6;
	b = foo(a);
	printf("n = ",b);
}
