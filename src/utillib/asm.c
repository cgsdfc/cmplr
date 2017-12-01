#include <stdio.h>

int main(void) {
	int foo=10,bar=15;
	
	__asm__ __volatile__ ("addl 	%%ebx %%eax" 
		: "=eax"(foo) 		// ouput
		: "eax"(foo), "ebx"(bar)// input
		: "eax"			// modify
	);
	printf("foo+bar=%d\n", foo);
	return 0;
}
