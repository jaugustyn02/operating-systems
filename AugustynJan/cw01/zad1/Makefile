GCC = gcc -O0 -Wall -std=c17 -ggdb
GCCF = gcc -Wall
LIB_NAME = wc_lib
all:
	make static_lib
	make shared_lib

static_lib $(LIB_NAME).c:
	$(GCCF) -c $(LIB_NAME).c
	ar cr $(LIB_NAME).a $(LIB_NAME).o

shared_lib $(LIB_NAME).c:
	$(GCCF) -c -fPIC $(LIB_NAME).c
	$(GCCF) -shared -o $(LIB_NAME).so $(LIB_NAME).o

clean:
	rm -f *.o *.a *.so *.gch