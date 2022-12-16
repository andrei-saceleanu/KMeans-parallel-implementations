CC = gcc
CFLAGS = -Wall -fopenmp
LDFLAGS = -lm

all:openmp_kmeans

openmp_kmeans: main_openmp.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f openmp_kmeans
