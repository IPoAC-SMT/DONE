CC = gcc
CFLAGS = -Wall -Wextra

poc: poc.c composer.o
	$(CC) $(CFLAGS) poc.c composer.o -o poc

composer.o: composer.c
	$(CC) $(CFLAGS) -c composer.c

clean:
	rm -f poc *.o