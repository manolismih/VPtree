#include "bench_util.h"
#include "vptree.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define NUM_OF_TESTS 15

int main(int argc, char *argv[])
{
    /*
        argv[1] : N
        argv[2] : d
    */
    int N = atoi(argv[1]);
    int d = atoi(argv[2]);
    double *X = malloc(sizeof(double) * N * d);
    double averageTime = 0;
    clock_t start, end;

    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        sampleGen(N, d, (double(*)[d])X);

        start = clock();
        buildvp(X, N, d);
        end = clock();

        averageTime += ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    averageTime = averageTime / NUM_OF_TESTS;
    printf("Input %dx%d takes %lf seconds on average\n", N, d, averageTime);

    free(X);
}
