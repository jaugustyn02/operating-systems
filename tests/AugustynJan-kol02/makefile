CC=gcc
CFLAGS=-I. -Wall -pthread 
DEPS = buffer.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pc: main.o  
	gcc -o pc -Wall -pthread  main.c -I.

clean:
	rm pc *.o