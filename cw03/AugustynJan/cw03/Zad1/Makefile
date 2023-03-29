CC=gcc
CFLAGS=-Wall

all:
	make main.exe

main.exe: main.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o $@

test: main.exe
	./main.exe 10

clean:
	rm -f *.o *.exe