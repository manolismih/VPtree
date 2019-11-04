#include "bench_util.h"
#include "vptree.h"
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#define NUM_OF_TESTS 5

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
    struct timeval start, end;
    long testTimeusec;

    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        sampleGen(N, d, (double(*)[d])X);

        gettimeofday(&start, NULL);
        vptree *root = buildvp(X, N, d);
        gettimeofday(&end, NULL);

        testTimeusec = (end.tv_sec - start.tv_sec) * 1000000 
                    + (end.tv_usec - start.tv_usec);
        averageTime += ((double)testTimeusec) / 1000000;

        preOrderTraversal(root); //for cleanup
    }

    averageTime = averageTime / NUM_OF_TESTS;
    printf("Input %dx%d takes %lf seconds on average\n", N, d, averageTime);

    free(X);
}
