#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#define INF 1000000 // "infinity" distance  used to initialize min_dist array

// if centroids norm doesn't change more than threshold EPS
// exit the main while loop/end the computation
#define EPS 0.0001 
#define MASTER 0
// n - number of samples/data points
// d - dimensionality of one data point
// k - number of centroids to identify
int n, d, k;

// sample an uniform integer in the interval [a, b]
int sample_int(int a, int b)
{
    return a + (rand() % (b - a));
}

// sample an uniform float in the interval [a, b]
float sample_float(float a, float b)
{
    float u = (float)rand() / RAND_MAX;
    return (b - a) * u + a;
}

// compute Euclidean distance/ L2 norm between points a and b
// of dimensionality d
float dist(float *a, float *b, int d)
{
    float res = 0.0;
    int i;
    float x;
    for(i = 0; i < d; i++) {
        x = *a - *b;
        res += x * x;
        a++;
        b++; 
    }
    return res;
}

// given the mind array (min_dist of one point to any centroid chosen so far)
// the function selects the next centroid according to a weighted probability
// distribution (see README for a more detailed description) 
int choose_next_centroid(float *mind, int n) 
{
    float u = sample_float(0.0, 1.0);
    float s = 0.0;
    int i;
    for(i = 0; i < n; i++) {
        s = s + *mind;
        if(s >= u)
            return i;
    }
    return -1;
}

// if the current centroids don't differ that much from the previous
// centroids, return 1 (i.e. end the main algorithm)
int stop_condition(float *c, float *prevc, int d, int k)
{
    int i, j;
    float total = 0.0, diff;
    for(i = 0; i < k; i++) {
        diff = 0.0;
        for(j = 0; j < d; j++){
            diff += (*c - *prevc) * (*c - *prevc);
            c++;
            prevc++;
        }
        total += sqrt(diff);
    }   
    return (total <= EPS);
}

int main(int argc, char *argv[])
{
    srand(42);
    int i, j, idx, res, rank, noProcesses;
    float val, distance, s;
    FILE *fin;
    int provided, num;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
    
    if (rank == MASTER) {
        k = atoi(argv[2]);
        fin = fopen(argv[1], "r");
        fscanf(fin, "%d %d", &n, &d);
    }
    num = atoi(argv[3]);
    omp_set_num_threads(num);

    res = MPI_Bcast(&k, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    if (res) {
        fprintf(stderr, "Error send from process: %d\n", rank);
    }
    res = MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    if (res) {
        fprintf(stderr, "Error send from process: %d\n", rank);
    }
    res = MPI_Bcast(&d, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    if (res) {
        fprintf(stderr, "Error send from process: %d\n", rank);
    }

    int sz = k * d;
    float *points, *centroids, *prev_centroids, *min_dist, *local_centroids;
    int *min_idx;
    
    int start = rank * (double)n / noProcesses;
    int end = fmin((rank + 1) * (double)n / noProcesses, n);

    points = (float *)calloc(n * d, sizeof(float));
    min_idx = (int *)calloc(n, sizeof(int));
    min_dist = (float *)calloc(n, sizeof(float));
    centroids = (float *)calloc(k * d, sizeof(float));
    prev_centroids = (float *)calloc(k * d, sizeof(float));
    local_centroids = (float *)calloc(d, sizeof(float));

    if (rank == MASTER) {
        for( i = 0; i < n; i++) {
            for (j = 0; j < d; j++) {
                fscanf(fin, "%f", &val);
                points[d * i + j] = val;
            }
        }
    }

    res = MPI_Bcast(points, n * d, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
    if (res) {
        fprintf(stderr, "Error recv from process: %d\n", rank);
    }

    if (rank == MASTER) {

        // init first centroid
        idx = sample_int(0, n);
        memcpy(&centroids[0], &points[d * idx], d * sizeof(float));
        memset(min_dist, INF, n * sizeof(float));
        min_dist[idx] = 0.0;
        
        // choose in k-means++ style rest of initial k-1 centroids
        for (i = 1; i < k; i++) {
            for (j = 0; j < n; j++) {
                distance = dist(&points[d * j], &centroids[d * (i - 1)], d);
                if (distance * distance < min_dist[j]) {
                    min_dist[j] = distance * distance;
                }
            }
            s = 0.0;
            for (j = 0; j < n; j++) {
                s += min_dist[j];
            }
            for (j = 0; j < n; j++) {
                min_dist[j] /= s;
            }
            idx = choose_next_centroid(min_dist, n);
            memcpy(&centroids[d * i], &points[d * idx], d * sizeof(float));
        }
    }
    res = MPI_Bcast(centroids, k * d, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
    if (res) {
        fprintf(stderr, "Error recv from process: %d\n", rank);
    }
 
    // classic k-means algorithm
    while(1) {
        if (rank == MASTER) {
            memcpy(prev_centroids, centroids, d * k * sizeof(float));
        }
        #pragma omp parallel for default(shared) private(i,j,distance)
        for (i = start; i < end; i++) {
            min_dist[i] = INF;
            min_idx[i] = -1;
            for (j = 0; j < k; j++) {
                distance = dist(&points[d * i], &centroids[d * j], d);
                if (distance < min_dist[i]) {
                    min_dist[i] = distance;
                    min_idx[i] = j;
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank != MASTER) {
            res = MPI_Send(&start, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        	if (res) {
            	fprintf(stderr, "Error send from process: %d\n", rank);
        	}
            res = MPI_Send(&end, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        	if (res) {
            	fprintf(stderr, "Error send from process: %d\n", rank);
        	}
            res = MPI_Send(min_idx + start, end - start, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        	if (res) {
            	fprintf(stderr, "Error send from process: %d\n", rank);
        	}
        } else {
            for (int worker_rank = 1; worker_rank < noProcesses; worker_rank++) {
                int recv_start, recv_end;
                res = MPI_Recv(&recv_start, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD, &status);
                if (res) {
                    fprintf(stderr, "Error recv from process: %d\n", rank);
                }
                res = MPI_Recv(&recv_end, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD, &status);
                if (res) {
                    fprintf(stderr, "Error recv from process: %d\n", rank);
                }
                res = MPI_Recv(min_idx + recv_start, recv_end - recv_start, MPI_INT, worker_rank, 0, MPI_COMM_WORLD, &status);
                if (res) {
                    fprintf(stderr, "Error recv from process: %d\n", rank);
                }
            }
        }
        res = MPI_Bcast(min_idx, n, MPI_INT, MASTER, MPI_COMM_WORLD);
        if (res) {
            fprintf(stderr, "Error recv from process: %d\n", rank);
        }
    
        // update_centroids
        int ii, cnt, recv_cnt;
        for(i = 0; i < k; i++) {
            memset(local_centroids, 0, d * sizeof(float));
            if (rank == MASTER) {
                memset(&centroids[d * i], 0, d * sizeof(float));
            }
            cnt = 0;
            recv_cnt = 0;
            
            MPI_Barrier(MPI_COMM_WORLD);

            #pragma omp parallel for reduction(+:local_centroids[:sz]) reduction(+:cnt) default(shared) private(j, ii)
            for (j = start; j < end; j++) {
                if (min_idx[j] == i) {
                    cnt++;
                    for(ii = 0; ii < d; ii++)
                        local_centroids[ii] += points[d * j + ii];
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);

            if (rank == MASTER) {
                for(ii = 0; ii < d; ii++)
                    centroids[d * i + ii] += local_centroids[ii];
                for (int worker_rank = 1; worker_rank < noProcesses; worker_rank++) {
                    res = MPI_Recv(&recv_cnt, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD, &status);
                    if (res) {
                        fprintf(stderr, "Error recv from process: %d\n", rank);
                    }
                    cnt += recv_cnt;
                    memset(local_centroids, 0, d * sizeof(float));
                    res = MPI_Recv(local_centroids, d, MPI_FLOAT, worker_rank, 0, MPI_COMM_WORLD, &status);
                    if (res) {
                        fprintf(stderr, "Error recv from process: %d\n", rank);
                    }
                    for(ii = 0; ii < d; ii++)
                        centroids[d * i + ii] += local_centroids[ii];
                }
                
            } else {
                res = MPI_Send(&cnt, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
                if (res) {
                    fprintf(stderr, "Error send from process: %d\n", rank);
                }
                res = MPI_Send(local_centroids, d, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD);
                if (res) {
                    fprintf(stderr, "Error send from process: %d\n", rank);
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);

            if (rank == MASTER) {
                if (cnt != 0) {
                    for(ii = 0; ii < d; ii++)
                        centroids[d * i + ii] /= cnt;
                } else {
                    idx = sample_int(0, n);
                    memcpy(&centroids[d * i], &points[d * idx], d * sizeof(float));
                }
            }
            res = MPI_Bcast(centroids, k * d, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
            if (res) {
                fprintf(stderr, "Error recv from process: %d\n", rank);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        int stop_cond;
        if (rank == MASTER) {
            stop_cond = stop_condition(centroids, prev_centroids, d, k);
        }
        res = MPI_Bcast(&stop_cond, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        if (res) {
            fprintf(stderr, "Error recv from process: %d\n", rank);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if(stop_cond)
            break;
    }

    if (rank == MASTER) {
        for (i = 0; i < d * k; i++) {
            printf("%.2f ", centroids[i]);
            if ((i > 0) && (i % d == (d - 1)))
                printf("\n");
        }
        fclose(fin);
    }

    free(points);
    free(centroids);
    free(prev_centroids);
    free(min_idx);
    free(min_dist);
    free(local_centroids);

    MPI_Finalize();

    return 0;
}
