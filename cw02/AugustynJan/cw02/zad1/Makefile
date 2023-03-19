CC=gcc
CFLAGS=-Wall

all:
	make tr_lib.exe
	make tr_sys.exe

tr_lib.exe: tr_lib.c
	$(CC) $(CFLAGS) -c tr_lib.c
	$(CC) $(CFLAGS) tr_lib.o -o $@

tr_sys.exe: tr_sys.c
	$(CC) $(CFLAGS) -c tr_sys.c
	$(CC) $(CFLAGS) tr_sys.o -o $@

test: tr_lib.exe tr_sys.exe
	./tr_lib.exe a @ sample lib
	./tr_sys.exe a @ sample sys
	tr a @ < sample > tmp
	diff -s tmp lib
	diff -s tmp sys

clean:
	rm -f *.o *.exe tmp lib sys