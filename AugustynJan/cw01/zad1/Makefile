GCC=gcc -O0 -Wall -std=c17 -ggdb
CCF=gcc -Wall

all:
	make wc_lib.o
	make wc_lib.so

# static
wc_lib.o: wc_lib.c wc_lib.h
	$(CCF) -c wc_lib.c
	ar cr wc_lib.a wc_lib.o

# shared
wc_lib.so: wc_lib.c wc_lib.h
	$(CCF) -c -fPIC wc_lib.c
	$(CCF) -shared -o wc_lib.so wc_lib.o

# test
library_test: library_test.c wc_lib.o
	$(CCF) -o $@ $^

clean:
	rm -f *.o *.a *.so library_test