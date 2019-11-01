#include "vptree.h"
#include "details.h"
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

//Globally defined variables for easy data access by threads
int *idArr;
double *distArr;
double *Y; //data array
int N, D;  //data dimensions

//Structure for argument passing for threads
typedef struct threadArgs
{
    int threadId;
    vptree *node;
    int start, end;
} threadArgs;

void *threadRecBuildTree(void *argumets);

void recursiveBuildTree(
    vptree *node,
    int start, int end,
    int n, int d,
    int idArr[n],
    double distArr[n],
    double dataArr[n][d])
{
    pthread_t threadInner;
    threadArgs *args;
    void *status;

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

    for (int i = start; i <= end; i++)
        distArr[i] = sqr(node->vp[0] - dataArr[idArr[i]][0]);
    for (int i = start; i <= end; i++)
        for (int j = 1; j < d; j++)
            distArr[i] += sqr(node->vp[j] - dataArr[idArr[i]][j]);

    quickSelect((start + end) / 2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = sqrt(distArr[(start + end) / 2]);
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));

    //create the arguments to pass
    args = malloc(sizeof(threadArgs));
    args->start = start;
    args->end = end;
    args->node = node;
    pthread_create(&threadInner, NULL, threadRecBuildTree, (void *)args);
    if (end > start)
        recursiveBuildTree(node->outer, (start + end) / 2 + 1, end, n, d, idArr, distArr, dataArr);
    else
        node->outer = NULL;

    //wait for thread to join
    pthread_join(threadInner, &status);
}

vptree *buildvp(double *X, int n, int d)
{
    vptree *root = malloc(sizeof(vptree));
    idArr = malloc(n * sizeof(int));
    distArr = malloc(n * sizeof(double));
    Y = X;
    N = n;
    D = d;

    for (int i = 0; i < n; i++)
        idArr[i] = i;

    recursiveBuildTree(root, 0, n - 1, n, d, idArr, distArr, (double(*)[d])X);

    free(idArr);
    free(distArr);
    return root;
}

void *threadRecBuildTree(void *argumets)
{
    threadArgs *args = (threadArgs *)argumets;

    recursiveBuildTree(args->node->inner, args->start, (args->start + args->end) / 2, N, D, idArr, distArr, (double(*)[D])Y);

    return (void *)0;
}
///////////////////////////////////////////////////////////
vptree *getInner(vptree *T) { return T->inner; }
vptree *getOuter(vptree *T) { return T->outer; }
double getMD(vptree *T) { return T->md; }
double *getVP(vptree *T) { return T->vp; }
int getIDX(vptree *T) { return T->idx; }