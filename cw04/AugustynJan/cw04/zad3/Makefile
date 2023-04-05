CC=gcc
CFLAGS=-Wall

all:
	make sender.exe
	make catcher.exe

sender.exe: sender.c
	$(CC) $(CFLAGS) -c sender.c
	$(CC) $(CFLAGS) -o $@ sender.o

catcher.exe: catcher.c
	$(CC) $(CFLAGS) -c catcher.c
	$(CC) $(CFLAGS) -o $@ catcher.o

test: sender.exe catcher.exe


clean:
	rm -f *.o *.exe