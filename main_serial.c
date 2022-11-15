#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define INF 1000000
#define EPS 0.0001

int n, d, k;


int sample_int(int a, int b)
{
    return a + (rand() % (b - a));
}

float sample_float(float a, float b)
{
    float u = (float)rand() / RAND_MAX;
    return (b - a) * u + a;
}

float dist(float *a, float *b, int d)
{
    float res = 0.0;
    int i;
    for(i = 0; i < d; i++){
        res += (*a - *b) * (*a - *b);
        a++;
        b++; 
    }
    return res;
}

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

void compute_min_idx(float *min_dist, int *min_idx, float *x, float *centroids){
    int i, j;
    float distance;
    for(i = 0; i < n; i++){
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

void update_centroids(float *x, float *centroids, int *min_idx)
{
    int i, j, ii, cnt, idx;
    for(i = 0; i < k; i++){
        memset(&centroids[d * i], 0, d * sizeof(float));
        cnt = 0;
        for(j = 0; j < n; j++){
            if(min_idx[j] == i){
                cnt++;
                for(ii = 0; ii < d; ii++)
                    centroids[d * i + ii] += x[d * j + ii];
            }
        }
        if(cnt!=0){
            for(ii = 0; ii < d; ii++)
                centroids[d * i + ii] /= cnt;
        }else{
            idx = sample_int(0, n);
            memcpy(&centroids[d * i], &x[d * idx], d * sizeof(float));
        }
    }
}


int main(int argc, char *argv[])
{
    srand(42);
    int i, j, idx;
    float val, distance, s;
    k = atoi(argv[2]);
    
    FILE *fin;
    fin = fopen(argv[1], "r");
    fscanf(fin, "%d %d", &n, &d);
    float *x, *centroids, *prev_centroids, *min_dist;
    int *min_idx;
    
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

    clock_t interval;
    interval = clock();

//init first
    idx = sample_int(0, n);
    memcpy(&centroids[0], &x[d * idx], d * sizeof(float));
    memset(min_dist, INF, n * sizeof(float));
    min_dist[idx] = 0.0;

//choose in k-means++ style rest of initial k-1 centroids
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

// classic k-means algorithm
    while(1){
        memcpy(prev_centroids, centroids, d * k * sizeof(float));
        compute_min_idx(min_dist, min_idx, x, centroids);
        update_centroids(x, centroids, min_idx);

        if(stop_condition(centroids, prev_centroids, d, k))
            break;
    }
    interval = clock() - interval;
    double elapsed_time = ((double)interval) / CLOCKS_PER_SEC;
    printf("Time: %.3f \n", elapsed_time);
    
    for(i = 0; i<d*k;i++){
        printf("%.2f ", centroids[i]);
        if((i > 0) && (i % d == (d-1)))
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