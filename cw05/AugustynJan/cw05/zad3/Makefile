CC=gcc
CFLAGS=-Wall
NAME=integral_exec
NAME2=calc

all:
	make $(NAME).exe
	make $(NAME2).exe

$(NAME).exe: $(NAME).c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -o $@ $(NAME).o

$(NAME2).exe: $(NAME2).c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -o $@ $(NAME2).o

test: $(NAME).exe $(NAME2).exe
	rm -f "report.txt"
	./$(NAME).exe 0.00001 5000
	./$(NAME).exe 0.000005 7000
	./$(NAME).exe 0.000001 6000
	./$(NAME).exe 0.0000005 5000
clean:
	rm -f *.exe *.o