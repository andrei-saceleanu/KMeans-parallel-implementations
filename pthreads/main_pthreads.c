#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include<pthread.h>

#define INF 1000000 // "infinity" distance  used to initialize min_dist array
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// if centroids norm doesn't change more than threshold EPS
// exit the main while loop/end the computation
#define EPS 0.0001 

typedef struct
{
    int id;
    pthread_barrier_t* bar;
    pthread_mutex_t* lock;
} thread_struct;

// n - number of samples/data points
// d - dimensionality of one data point
// k - number of centroids to identify
// p - number of threads
int n, d, k, p;
float *x, *centroids, *prev_centroids, *min_dist;
int *min_idx;
int stop_cond;

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
    for(i = 0; i < d; i++){
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
int choose_next_centroid(float *mind, int n){
    float u = sample_float(0.0, 1.0);
    float s = 0.0;
    int i;
    for(i = 0; i < n; i++){
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
    for(i = 0; i < k; i++){
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

// find, for each datapoint, which cluster it belongs to
// find the closest centroid
void compute_min_idx(float *min_dist, int *min_idx, float *x, float *centroids, int tid){
    int i, j;
    float distance;

    int start = tid * (double)n / p;
    int end = MIN((tid + 1) * (double)n / p, n);

    for(i = start; i < end; i++){
        min_dist[i] = INF;
        min_idx[i] = -1;
        for(j = 0; j < k; j++){
            distance = dist(&x[d * i], &centroids[d * j], d);
            if(distance < min_dist[i]){
                min_dist[i] = distance;
                min_idx[i] = j;
            }
        }
    }
}

int cnt;
// the new centroids set is computed
// the i-th centroid is the average of the datapoints assigned to
// i-th cluster (min_idx == i)
// if one centroid did not have at least one point assigned to it,
// resample from the available points
void update_centroids(float *x, float *centroids, int *min_idx, thread_struct* thread_params)
{
    int tid = thread_params->id;
    int start = tid * (double)n / p;
    int end = MIN((tid + 1) * (double)n / p, n); 
    pthread_barrier_t* bar = thread_params->bar;
    pthread_mutex_t* lock = thread_params->lock;

    int i, j, ii, idx, local_cnt;
    float local_centroids[d];
    
    for(i = 0; i < k; i++){
        if (tid == 0) {
            memset(&centroids[d * i], 0, d * sizeof(float));
            cnt = 0;
        }

        pthread_barrier_wait(bar);

        local_cnt = 0;
        for (ii = 0; ii < d; ++ii){
            local_centroids[ii] = 0.0;
        }

        for(j = start; j < end; j++){
            if(min_idx[j] == i){
                local_cnt++;
                for(ii = 0; ii < d; ii++)
                    local_centroids[ii] += x[d * j + ii];
            }
        }

        pthread_mutex_lock(lock);
        cnt += local_cnt;
        for(ii = 0; ii < d; ++ii){
            centroids[d * i + ii] += local_centroids[ii];
        }
        pthread_mutex_unlock(lock);

        pthread_barrier_wait(bar);

        if (tid == 0) {
            if(cnt!=0){
                for(ii = 0; ii < d; ii++)
                    centroids[d * i + ii] /= cnt;
            }else{
                idx = sample_int(0, n);
                memcpy(&centroids[d * i], &x[d * idx], d * sizeof(float));
            }            
        }
    }
}

void *kmeans(void *arg) {
    thread_struct* thread_params = (thread_struct *)arg;
    int thread_id = thread_params->id;
    pthread_barrier_t* bar = thread_params->bar;
// classic k-means algorithm
   do {
        if (thread_id == 0) {
            memcpy(prev_centroids, centroids, d * k * sizeof(float));
        }

        pthread_barrier_wait(bar);
        compute_min_idx(min_dist, min_idx, x, centroids, thread_id);
        pthread_barrier_wait(bar);
        update_centroids(x, centroids, min_idx, thread_params);
        pthread_barrier_wait(bar);

        if (thread_id == 0) {
            stop_cond = stop_condition(centroids, prev_centroids, d, k);
        }
        pthread_barrier_wait(bar);

    } while(!stop_cond);

      pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    srand(42);
    int i, j, idx;
    float val, distance, s;
    k = atoi(argv[2]);
    p = atoi(argv[3]);

    thread_struct thread_params[p];
    pthread_t tid[p];

    pthread_barrier_t bar;
    pthread_mutex_t lock;
    pthread_barrier_init(&bar, NULL, p);
    pthread_mutex_init(&lock, NULL);
    
    FILE *fin;
    fin = fopen(argv[1], "r");
    fscanf(fin, "%d %d", &n, &d);
    
    x = (float *)malloc(n * d * sizeof(float));
    min_idx = (int *)malloc(n * sizeof(int));
    min_dist = (float *)malloc(n * sizeof(float));
    centroids = (float *)malloc(k * d * sizeof(float));
    prev_centroids = (float *)malloc(k * d * sizeof(float));
    
    for(i = 0; i < n; i++){
        for(j = 0; j < d; j++){
            fscanf(fin, "%f", &val);
            x[d*i + j] = val;
        }
    }

// init first centroid
    idx = sample_int(0, n);
    memcpy(&centroids[0], &x[d * idx], d * sizeof(float));
    memset(min_dist, INF, n * sizeof(float));
    min_dist[idx] = 0.0;

// choose in k-means++ style rest of initial k-1 centroids
    for(i = 1; i < k; i++){
        for(j = 0; j < n; j++){
            distance = dist(&x[d*j], &centroids[d*(i-1)], d);
            if(distance * distance < min_dist[j]){
                min_dist[j] = distance * distance;
            }
        }
        
        s = 0.0;
        for(j = 0; j < n; j++){
            s += min_dist[j];
        }
        for(j = 0; j < n; j++){
            min_dist[j] /= s;
        }

        idx = choose_next_centroid(min_dist, n);
        memcpy(&centroids[d * i], &x[d * idx], d * sizeof(float));
    }

// se creeaza thread-urile
    for (i = 0; i < p; i++) {
		thread_params[i] = (thread_struct){i, &bar, &lock};
		pthread_create(&tid[i], NULL, kmeans, &thread_params[i]);
	}

// se asteapta thread-urile
	for (i = 0; i < p; i++) {
		pthread_join(tid[i], NULL);
	}

    pthread_barrier_destroy(&bar);
    pthread_mutex_destroy(&lock);
    
    for(i = 0; i < d * k; i++){
        printf("%.2f ", centroids[i]);
        if((i > 0) && (i % d == (d - 1)))
            printf("\n");
    }

    free(x);
    free(centroids);
    free(prev_centroids);
    free(min_idx);
    free(min_dist);
    fclose(fin);
    return 0;
}
