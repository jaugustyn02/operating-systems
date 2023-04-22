CC=gcc
CFLAGS=-Wall
P1=server
P2=client

all:
	make $(P1).exe
	make $(P2).exe

$(P1).exe: $(P1).c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -o $@ $(P1).o

$(P2).exe: $(P2).c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -o $@ $(P2).o

test: $(P1).exe $(P2).exe
	./$(P1).exe

clean:
	rm -f *.exe *.o