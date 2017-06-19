all: tokenizer
cflags=-g3 

tokenizer: tokenizer.o Makefile char_buffer.c token.c state.h token.h
	ctags *.c
	gcc $(cflags) tokenizer.c char_buffer.c token.c  -o tokenizer

.PHONY: clean

clean:
	rm *.o
	rm tokenizer


