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

server: $(P1).exe $(P2).exe
	./$(P1).exe './server' 2137

client1: $(P1).exe $(P2).exe
	./$(P2).exe Figo -u './server'

client2: $(P1).exe $(P2).exe
	./$(P2).exe Fagot -w $(shell hostname -I) 2137

clean:
	rm -f *.exe *.o server