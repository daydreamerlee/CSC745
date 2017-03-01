CC = cc -std=c99
CFLAGES = -Wall -pthread

all: run

run: page_simulation.c
	$(CC) $(CFLAGES) page_simulation.c -o run 

clean:
	rm -f *.o run