CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm
TEST_NO = 1
NP = 4
INPUT_FILE = input_data/points$(TEST_NO).txt
K = 5

build: openmp_kmeans pthreads_kmeans mpi_kmeans serial_kmeans

openmp_kmeans: openmp/main_openmp.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -fopenmp

pthreads_kmeans: pthreads/main_pthreads.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -lpthread

mpi_kmeans: mpi/main_mpi.c
	mpicc $^ -o $@ $(CFLAGS) $(LDFLAGS)

serial_kmeans: serial/main_serial.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run_openmp: openmp_kmeans
	./openmp_kmeans $(INPUT_FILE) $(K) $(NP)

run_pthreads: pthreads_kmeans
	./pthreads_kmeans $(INPUT_FILE) $(K) $(NP)

run_mpi: mpi_kmeans
	mpirun -np $(NP) mpi_kmeans $(INPUT_FILE) $(K)

run_serial: serial_kmeans
	./serial_kmeans $(INPUT_FILE) $(K)

.PHONY: clean

clean:
	rm -f openmp_kmeans pthreads_kmeans mpi_kmeans serial_kmeans
