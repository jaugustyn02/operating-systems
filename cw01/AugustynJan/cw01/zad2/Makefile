CCF=gcc -Wall
LD_LIB_PATH=/usr/lib64

.PHONY: all clean

all: libwc.o libwc.so repl_static.exe repl_shared.exe repl_dynamic.exe

# Library
# static
libwc.o: libwc.c libwc.h
	$(CCF) -c $<
	ar cr libwc.a $@

# shared
libwc.so: libwc.c libwc.h
	$(CCF) -lc -fPIC -shared -o $@ $<
# end

# Test program
repl_static.exe: repl.c libwc.c
	$(CCF) -o $@ $< libwc.a

repl_shared.exe: repl.c libwc.c
	#$(CCF) -o $@ $< libwc.so -lwc
	$(CCF) -o $@ $< libwc.so -lwc -L. -Wl,-rpath,.

repl_dynamic.exe: repl.c libwc.c
	$(CCF) -o $@ $< -ldl -DDYNAMIC

# copies libwc.so to /usr/lib64 - default LD_LIBRARY_PATH on Fedora 36
cp_shared_lib: libwc.so
	sudo cp libwc.so $(LD_LIB_PATH)
# end

clean:
	rm -f *.o *.a *.so *.exe