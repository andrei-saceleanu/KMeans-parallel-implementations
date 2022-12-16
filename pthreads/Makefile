CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm -lpthread

all:pthreads_kmeans

pthreads_kmeans: main_pthreads.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f pthreads_kmeans
