CC=gcc
CFLAGS=-Wall

all:
	make main.exe

main.exe: main.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o $@

test: main.exe
	# Empty:
	./main.exe ./empty_dir
	#
	# Few files:
	./main.exe ../Zad1
	# Many files:
	./main.exe .

clean:
	rm -f *.o *.exe