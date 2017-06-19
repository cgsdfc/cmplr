all: tokenizer
cflags=-g3 

tokenizer: tokenizer.o Makefile char_buffer.c token.c
	ctags *.c
	gcc $(cflags) tokenizer.c char_buffer.c token.c  -o tokenizer

.PHONY: clean

clean:
	rm *.o
	rm tokenizer


