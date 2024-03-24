CC = gcc
CFLAGS = -Wall -Wextra

poc: poc.c netlib.o
	$(CC) $(CFLAGS) poc.c netlib.o -o poc

netlib.o: netlib.c
	$(CC) $(CFLAGS) -c netlib.c

clean:
	rm -f poc *.o