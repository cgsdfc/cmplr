all: tokenizer
cflags=-g3 
objs=tokenizer.o char_buffer.o state.o token.o
head=*.h
src=*.c

char_buffer.o: char_buffer.c

state.o:state.c

token.o:token.c

tokenizer.o:tokenizer.c

base.o: state.o token.o char_buffer.o tokenizer.o
	gcc state.o token.o char_buffer.o tokenizer.o -o base.o

transfer: transfer.o base.o
	gcc base.o transfer.o -o transfer


cprd_transfer: cprd_transfer.o base.o
	gcc base.o cprd_transfer.o -o cprd_transfer

.PHONY: clean

clean:
	rm *.o
	rm tokenizer


