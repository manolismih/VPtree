#include "vptree.h"
#include "details.h"
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>

#define MAX_THREADS 2
//global active thread counter
int activeThreads = 1;
pthread_mutex_t mutexCounter;

inline void updateThreadCounter(int x)
{
    pthread_mutex_lock(&mutexCounter);
    activeThreads += x;
    pthread_mutex_unlock(&mutexCounter);
}

//Globally defined variables for easy data access by threads
int *idArr;
double *distArr;
double *Y; //data array
int N, D;  //data dimensions

//Structure for argument passing for threadDistCalc
typedef struct threadArgs
{
    int start, end;
    vptree *node;
} threadArgs;

void *threadRecBuildTree(void *arguments);
void *threadDistCalc(void *arguments);

////////////////////////////////////////////////////////////////////////

void distCalc(double *vp, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])Y;
    for (int i = start; i <= end; i++)
        distArr[i] = sqr(vp[0] - dataArr[idArr[i]][0]);
    for (int i = start; i <= end; i++)
        for (int j = 1; j < D; j++)
            distArr[i] += sqr(vp[j] - dataArr[idArr[i]][j]);
};

////////////////////////////////////////////////////////////////////////

void recursiveBuildTree(vptree *node, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])Y;
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

    int freeThreads = MAX_THREADS - activeThreads;
    if (freeThreads <= 0)
        distCalc(node->vp, start, end); //sequential calculation
    else
    {
        updateThreadCounter(+freeThreads);
        int workPerThread = (end - start + 1) / (freeThreads + 1);
        //array of thread objects for distArr calculation
        pthread_t threadObj[freeThreads];
        //we need an array of different args, each one for each thread
        threadArgs Args[freeThreads];

        for (int id = 0; id < freeThreads; id++)
        {
            Args[id].start = start + id * workPerThread;
            Args[id].end = Args[id].start + workPerThread - 1;
            Args[id].node = node;
            pthread_create(threadObj + id, NULL, threadDistCalc, (void *)(Args + id));
        }
        //current thread does it's share + the leftovers till end
        distCalc(node->vp, start + freeThreads * workPerThread, end);

        for (int id = 0; id < freeThreads; id++)
            pthread_join(threadObj[id], NULL);
        updateThreadCounter(-freeThreads);
    }

    quickSelect((start + end) / 2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = sqrt(distArr[(start + end) / 2]);
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));

    int doThreading = activeThreads < MAX_THREADS;
    pthread_t threadInner;
    if (doThreading)
    {
        updateThreadCounter(+1);
        //create the arguments to pass
        threadArgs innerArgs;
        innerArgs.node = node->inner;
        innerArgs.end = (start + end) / 2;
        innerArgs.start = start;

        pthread_create(&threadInner, NULL, threadRecBuildTree, (void *)&innerArgs);
    }
    else
        recursiveBuildTree(node->inner, start, (start + end) / 2);

    if (end > start)
        recursiveBuildTree(node->outer, (start + end) / 2 + 1, end);
    else
        node->outer = NULL;

    //wait for thread to join
    if (doThreading)
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

    for (int i = 0; i < N; i++)
        idArr[i] = i;

    pthread_mutex_init(&mutexCounter, NULL);

    recursiveBuildTree(root, 0, n - 1);

    pthread_mutexattr_destroy(&mutexCounter);
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
    distCalc(args->node->vp, args->start, args->end);
    pthread_exit((void *)0);
    return (void *)0;
}

///////////////////////////////////////////////////////////

vptree *getInner(vptree *T) { return T->inner; }
vptree *getOuter(vptree *T) { return T->outer; }
double getMD(vptree *T) { return T->md; }
double *getVP(vptree *T) { return T->vp; }
int getIDX(vptree *T) { return T->idx; }
