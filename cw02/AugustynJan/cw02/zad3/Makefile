CC=gcc
CFLAGS=-Wall

all:
	make wcdir.exe

wcdir.exe: wcdir.c
	$(CC) $(CFLAGS) -c wcdir.c
	$(CC) $(CFLAGS) wcdir.o -o $@

test: wcdir.exe
	find . -maxdepth 1 -type f | xargs wc --bytes
	./wcdir.exe

clean:
	rm -f *.o *.exe