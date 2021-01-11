CC=gcc
CFLAGS=-Wall -g -std=c99
CLINK=$(CC)

libgrades.so: grades.o
	$(CLINK) -shared grades.o -o libgrades.so -llinked-list -L.

grades.o: grades.c grades.h
	$(CC) -c -fpic grades.c

clean:
	rm -f *.o *.so