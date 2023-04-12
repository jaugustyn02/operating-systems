CC=gcc
CFLAGS=-Wall
NAME=mail_manager

all:
	make $(NAME).exe

$(NAME).exe: $(NAME).c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -o $@ $(NAME).o

clean:
	rm -f *.exe *.o
