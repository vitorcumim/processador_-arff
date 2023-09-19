CFLAGS=-Wall -g
CC=gcc

objects=main.o arff.o

all:$(objects)
	$(CC) -o arff $(objects)

main.o:main.c 
	$(CC) -c $(CFLAGS) main.c 

arff.o:arff.c
	$(CC) -c $(CFLAGS) arff.c 

clean:
	rm -f $(objects)

purge:
	rm -f $(objects) arff