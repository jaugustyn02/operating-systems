CC=gcc
CFLAGS=-Wall

all:
	make sig_fork.exe
	make sig_exec.exe

sig_fork.exe: sig_fork.c
	$(CC) $(CFLAGS) -c sig_fork.c
	$(CC) $(CFLAGS) -o $@ sig_fork.o

sig_exec.exe: sig_exec.c
	$(CC) $(CFLAGS) -c sig_exec.c
	$(CC) $(CFLAGS) -o $@ sig_exec.o

test: sig_fork.exe sig_exec.exe
	./sig_fork.exe ignore > fork_result.txt
	./sig_fork.exe handler >> fork_result.txt
	./sig_fork.exe mask >> fork_result.txt
	./sig_fork.exe pending >> fork_result.txt
	./sig_exec.exe ignore > exec_result.txt
	./sig_exec.exe mask >> exec_result.txt
	./sig_exec.exe pending >> exec_result.txt

clean:
	rm -f *.o *.exe