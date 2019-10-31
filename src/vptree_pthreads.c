//vptree_pthreads.c
#include "vptree.h"
#include "details.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 2

typedef struct threadDistCalcArgs
{
    int threadId;
    vptree *node;
    int start, end, d;
    int *idArr;
    double *distArr;
    double **dataArr;
} threadDistCalcArgs;

///////////////////////////////////////////////////////////////////////////////////

void *threadDistCalc(void *arguments)
{
    threadDistCalcArgs *args = (threadDistCalcArgs *)arguments;

    int offset = args->threadId;
    vptree *node = args->node;
    int start = args->start;
    int end = args->end;
    int d = args->d;
    int *idArr = args->idArr;
    double *distArr = args->distArr;
    double **dataArr = args->dataArr;

    int range = (start - end) / NUM_OF_THREADS;

    int threadStart = start + offset * range;
    int threadEnd = start + (offset + 1) * range;

    for (int i = threadStart; i < threadEnd; i++)
        distArr[i] = sqr(node->vp[0] - dataArr[idArr[start]][0]);
    for (int i = threadStart; i < threadEnd; i++)
        for (int j = 1; j < d; j++)
            distArr[i] += sqr(node->vp[j] - dataArr[idArr[start]][j]);

    pthread_exit((void *)0);
};

///////////////////////////////////////////////////////////////////////////////////

void parallelDistCalc(
    vptree *node,
    int start, int end,
    int n, int d,
    int idArr[n],
    double distArr[n],
    double dataArr[n][d])
{
    //Threads declaration
    pthread_t threads[NUM_OF_THREADS];
    int rc;       //stores the return value of pthread_create()
    void *status; //is used as an argument for pthread_join()

    //copy all data to "arguments structure"
    threadDistCalcArgs *args = malloc(sizeof(threadDistCalcArgs));
    args->node = node;
    args->start = start;
    args->end = end;
    args->d = d;
    args->idArr = idArr;
    args->distArr = distArr;
    args->dataArr = (double **)dataArr;

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        args->threadId = i;
        rc = pthread_create(&threads[i], NULL, threadDistCalc, (void *)args);
        if (rc)
        {
            printf("Error creating thread #%d, Exiting...\n", i);
            exit(1);
        }
    }

    //wait for threads to finish and join
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        rc = pthread_join(threads[i], &status);
        if (rc)
        {
            printf("Error joining thread #%d, Exiting\n", i);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////

void recursiveBuildTree(
    vptree *node,
    int start, int end,
    int n, int d,
    int idArr[n],
    double distArr[n],
    double dataArr[n][d])
{
    //consider X[ idArr[start] ] as vantage point
    node->idx = idArr[start];
    node->vp = dataArr[node->idx];

    if (start == end)
    {
        node->inner = node->outer = NULL;
        node->md = 0.0;
        return;
    }

    //calculate distances in parallel
    parallelDistCalc(node, start, end, n, d, idArr, distArr, dataArr);

    quickSelect((end - start) / 2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = distArr[(start + end) / 2];
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));
    recursiveBuildTree(node->inner, start, (start + end) / 2, n, d, idArr, distArr, dataArr);
    recursiveBuildTree(node->outer, (start + end) / 2 + 1, end, n, d, idArr, distArr, dataArr);
};

///////////////////////////////////////////////////////////////////////////////////

vptree *buildvp(double *X, int n, int d)
{
    vptree *root = malloc(sizeof(vptree));
    int *idArr = malloc(n * sizeof(int));
    double *distArr = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
        idArr[i] = i;
    recursiveBuildTree(root, 0, n - 1, n, d, idArr, distArr, (double(*)[d])X);

    free(idArr);
    free(distArr);
    return root;
}
