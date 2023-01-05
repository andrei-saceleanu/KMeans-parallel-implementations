CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm
NP = 4
OMP_TH = 1
INPUT_FILE = ../input_data/points1.txt
K = 5


hybrid_kmeans: main_hybrid.c
	mpicc $^ -o $@ $(CFLAGS) $(LDFLAGS) -fopenmp

run_hybrid: hybrid_kmeans
	mpirun -np $(NP) hybrid_kmeans $(INPUT_FILE) $(K) $(OMP_TH)

.PHONY: clean

clean:
	rm -f hybrid_kmeans
