CC=gcc
CFLAGS=-Wall

all:
	make wcdir_rec.exe

wcdir_rec.exe: wcdir_rec.c
	$(CC) $(CFLAGS) -c wcdir_rec.c
	$(CC) $(CFLAGS) wcdir_rec.o -o $@

test: wcdir_rec.exe
	./wcdir_rec.exe .
	find . -type f -print | xargs wc --bytes

clean:
	rm -f *.o *.exe