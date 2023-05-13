CC = gcc
CFLAGS = -Wall
LIBS = -lpthread

all: santa_problem

santa_problem: santa_problem.c
	$(CC) $(CFLAGS) santa_problem.c -o santa_problem $(LIBS)

test: santa_problem
	./santa_problem

clean:
	rm -f santa_problem