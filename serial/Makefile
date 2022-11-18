CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm

all:serial_kmeans

serial_kmeans: main_serial.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f serial_kmeans
