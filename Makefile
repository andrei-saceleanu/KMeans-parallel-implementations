CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm

all:serial

serial: main_serial.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


.PHONY: clean

clean:
	rm -f serial