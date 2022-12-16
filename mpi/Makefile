CC = mpicc
CFLAGS = -Wall
LDFLAGS = -lm
TEST_NO = 1
NP=4
INPUT_FILE=../input_data/points$(TEST_NO).txt
K=5

all:mpi_kmeans

mpi_kmeans: main_mpi.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: mpi_kmeans
	mpirun -np $(NP) ./mpi_kmeans $(INPUT_FILE) $(K)

.PHONY: clean

clean:
	rm -f mpi_kmeans
