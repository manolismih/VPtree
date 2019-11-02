#include "vptree.h"
#include "details.h"
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>

#define ELEMS_PER_THREAD 16

//Globally defined variables for easy data access by threads
int *idArr;
double *distArr;
double *Y; //data array
int N, D;  //data dimensions

//Structure for argument passing for threads
typedef struct threadArgs
{
    int threadId;
    vptree* node;
    int start, end;
} threadArgs;

void *threadRecBuildTree(void* arguments);
void *threadDistCalc(void* arguments);

////////////////////////////////////////////////////////////////////////

void distCalc(vptree* node, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])Y;
    for (int i = start; i <= end; i++)
        distArr[i] = sqr(node->vp[0] - dataArr[idArr[i]][0]);
    for (int i = start; i <= end; i++)
        for (int j = 1; j < D; j++)
            distArr[i] += sqr(node->vp[j] - dataArr[idArr[i]][j]);
};

////////////////////////////////////////////////////////////////////////

void recursiveBuildTree(vptree* node, int start, int end)
{
    double (*dataArr)[D] = (double(*)[D])Y;
    pthread_t threadInner;
    threadArgs innerArgs;
    pthread_t *threadDist = NULL; //array of threads for distArr calculation
    threadArgs *distArgs = NULL; //we need an array of different args, each one for each thread
    int numOfThreads;

    //consider X[ idArr[end] ] as vantage point
    node->idx = idArr[end];
    node->vp = dataArr[node->idx];

    if (start == end)
    {
        node->inner = node->outer = NULL;
        node->md = 0.0;
        return;
    }

    end--; //end is the vantage point, we're not dealing with it again

    numOfThreads = (end - start) / ELEMS_PER_THREAD;
    if (numOfThreads > 1)
    {
        threadDist = malloc(numOfThreads * sizeof(pthread_t));
        distArgs = malloc(numOfThreads * sizeof(threadArgs));

        for (int i = 0; i < numOfThreads; i++)
        {
            distArgs[i].node = node;
            distArgs[i].threadId = i;
            distArgs[i].start = start;
            distArgs[i].end = end;
            pthread_create(&threadDist[i], NULL, threadDistCalc, (void *)&distArgs[i]);
        }

        //wait for threads to finish their work

        for (int i = 0; i < numOfThreads; i++)
            pthread_join(threadDist[i], NULL);

        free(threadDist);
        free(distArgs);
    }
    else    
        distCalc(node, start, end); //sequential calculation of distArr

    quickSelect((start + end) / 2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = sqrt(distArr[ (start+end)/2 ] );
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));

    //create the arguments to pass
    innerArgs.node = node->inner;
    innerArgs.end = (start+end)/2;
    innerArgs.start = start;

    //~ recursiveBuildTree(node->inner,start,(start+end)/2);
    pthread_create(&threadInner, NULL, threadRecBuildTree, (void *)&innerArgs); //build inner tree in parallel

    if (end > start)
        recursiveBuildTree(node->outer, (start+end)/2 +1, end);
    else
        node->outer = NULL;

    //wait for thread to join
    pthread_join(threadInner, NULL);
}

////////////////////////////////////////////////////////////////////////

vptree *buildvp(double *X, int n, int d)
{
    vptree *root = malloc(sizeof(vptree));
    idArr = malloc(n * sizeof(int));
    distArr = malloc(n * sizeof(double));
    Y = X;
    N = n;
    D = d;
    
    for (int i=0; i<N; i++) idArr[i] = i;
    
    recursiveBuildTree(root, 0, n-1);

    free(idArr);
    free(distArr);
    return root;
}

////////////////////////////////////////////////////////////////////////

void *threadRecBuildTree(void *arguments)
{
    threadArgs *args = (threadArgs *)arguments;

    recursiveBuildTree(args->node, args->start, args->end);
    pthread_exit((void *)0);
    return (void *)0;
}

////////////////////////////////////////////////////////////////////////

void *threadDistCalc(void *arguments)
{
    threadArgs *args = (threadArgs *)arguments;

    int threadStart = args->start + (args->threadId) * ELEMS_PER_THREAD;
    int threadEnd = args->start +
                    (args->threadId + 1) * ELEMS_PER_THREAD - 1;

    if (threadEnd + ELEMS_PER_THREAD > args->end)
        threadEnd = args->end;

    distCalc(args->node, threadStart, threadEnd);

    pthread_exit((void *)0);
    return (void *)0;
}
///////////////////////////////////////////////////////////
vptree *getInner(vptree *T) { return T->inner; }
vptree *getOuter(vptree *T) { return T->outer; }
double getMD(vptree *T) { return T->md; }
double *getVP(vptree *T) { return T->vp; }
int getIDX(vptree *T) { return T->idx; }
