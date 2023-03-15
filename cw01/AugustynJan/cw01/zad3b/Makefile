CCF=gcc -Wall
LD_LIB_PATH=/usr/lib64
OL=O0

.PHONY: all clean cp_shared_lib

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
repl_static.exe: repl.c libwc.o
	$(CCF) -$(OL) -o $@ $< libwc.a

repl_shared.exe: repl.c libwc.so
	$(CCF) -$(OL) -o $@ $< libwc.so -lwc

repl_dynamic.exe: repl.c libwc.c
	$(CCF) -$(OL) -o $@ $< -ldl -DDYNAMIC

# copies libwc.so to /usr/lib64 - default LD_LIBRARY_PATH on Fedora 36
cp_shared_lib: libwc.so
	sudo cp libwc.so $(LD_LIB_PATH)
# end

# Tests
run_tests:
	# static - O2, O3, Os
	make repl_static.exe OL=O2
	cat commands.txt | ./repl_static.exe > result_static_O2.txt
	cat result_static_O2.txt > report.txt
	make repl_static.exe OL=O3
	cat commands.txt | ./repl_static.exe > result_static_O3.txt
	cat result_static_O3.txt >> report.txt
	make repl_static.exe OL=Os
	cat commands.txt | ./repl_static.exe > result_static_Os.txt
	cat result_static_Os.txt >> report.txt

	# shared - O2, O3, Os
	make repl_shared.exe OL=O2
	cat commands.txt | ./repl_shared.exe > result_shared_O2.txt
	cat result_shared_O2.txt >> report.txt
	make repl_shared.exe OL=O3
	cat commands.txt | ./repl_shared.exe > result_shared_O3.txt
	cat result_shared_O3.txt >> report.txt
	make repl_shared.exe OL=Os
	cat commands.txt | ./repl_shared.exe > result_shared_Os.txt
	cat result_shared_Os.txt >> report.txt

	# dynamic - O2, O3, Os
	make repl_dynamic.exe OL=O2
	cat commands.txt | ./repl_dynamic.exe > result_dynamic_O2.txt
	cat result_dynamic_O2.txt >> report.txt
	make repl_dynamic.exe OL=O3
	cat commands.txt | ./repl_dynamic.exe > result_dynamic_O3.txt
	cat result_dynamic_O3.txt >> report.txt
	make repl_dynamic.exe OL=Os
	cat commands.txt | ./repl_dynamic.exe > result_dynamic_Os.txt
	cat result_dynamic_Os.txt >> report.txt
# end

clean:
	rm -f *.o *.a *.so *.exe