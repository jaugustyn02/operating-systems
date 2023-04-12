CC=gcc
CFLAGS=-Wall
NAME=integral_fork

all:
	make $(NAME).exe

$(NAME).exe: $(NAME).c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -o $@ $(NAME).o

test: $(NAME).exe
	rm -f "report.txt"
	./$(NAME).exe 0.00001 5000
	./$(NAME).exe 0.000005 7000
	./$(NAME).exe 0.000001 6000
	./$(NAME).exe 0.0000005 5000
clean:
	rm -f *.exe *.o