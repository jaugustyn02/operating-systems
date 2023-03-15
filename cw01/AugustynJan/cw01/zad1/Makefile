CCF=gcc -Wall

all: libwc.o libwc.so

# static
libwc.o: libwc.c libwc.h
	$(CCF) -c libwc.c
	ar cr libwc.a libwc.o

# shared
libwc.so: libwc.c libwc.h
	$(CCF) -c -fPIC libwc.c
	$(CCF) -shared -o libwc.so libwc.o

clean:
	rm -f *.o *.a *.so