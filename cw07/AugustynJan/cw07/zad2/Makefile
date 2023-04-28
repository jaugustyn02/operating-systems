CC=gcc
CFLAGS=-Wall
P1=barbershop
P2=client
P3=barber
OTHER=common.h semaphore.h shared_memory.h

all:
	make $(P1)
	make $(P2)
	make $(P3)

$(P1): $(P1).c $(OTHER)
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -lpthread -o $@ $(P1).o

$(P2): $(P2).c $(OTHER)
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -lpthread -o $@ $(P2).o

$(P3): $(P3).c $(OTHER)
	$(CC) $(CFLAGS) -c $^
	$(CC) $(CFLAGS) -lpthread -o $@ $(P3).o

test: $(P1) $(P2) $(P3)
	./$(P1)

clean:
	rm -f $(P1) $(P2) $(P3) *.o *.gch