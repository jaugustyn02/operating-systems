CC=gcc
CFLAGS=-Wall

all:
	make reverse.exe
	make reverse1024.exe

reverse.exe: reverse.c
	$(CC) $(CFLAGS) -c reverse.c
	$(CC) $(CFLAGS) reverse.o -o $@

reverse1024.exe: reverse1024.c
	$(CC) $(CFLAGS) -c reverse1024.c
	$(CC) $(CFLAGS) reverse1024.o -o $@

test: reverse.exe reverse1024.exe
	rm -f pomiar_zad_2.txt
	./reverse.exe binary_file tmp1
	./reverse.exe tmp1 tmp2
	diff -s tmp2 binary_file
	./reverse1024.exe binary_file tmp1
	./reverse1024.exe tmp1 tmp2
	diff -s tmp2 binary_file

test2:
	#tac < input_file | rev > output_file
    #./reverse.exe input_file tmp3
    #./reverse.exe tmp3 tmp4
    #diff -s tmp4 output_file

clean:
	rm -f *.o *.exe tmp* output_file