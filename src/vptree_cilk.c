#include "vptree.h"
#include "details.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#define MAX_THREADS 2
#define MIN_THREAD_WORK 8000

//Globally defined variables for easy data access by threads
int *idArr;
double *distArr;
double *Y; //data array
int N, D;  //data dimensions

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

    int workPerThread = (end - start + 1) / (MAX_THREADS);
    int threadStart, threadEnd;

    if (workPerThread < MIN_THREAD_WORK)
        distCalc(node->vp, start, end);
    else
    {
        for (int id = 0; id < MAX_THREADS - 1; id++)
        {
            threadStart = start + id * workPerThread;
            threadEnd = threadStart + workPerThread - 1;
            cilk_spawn distCalc(node->vp, threadStart, threadEnd);
        }
        //current thread does it's share + the leftovers till end
        distCalc(node->vp, start + (MAX_THREADS - 1) * workPerThread, end);

        cilk_sync;
    }

    quickSelect((start + end) / 2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = sqrt(distArr[(start + end) / 2]);
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));
    if (end - start + 1 > MIN_THREAD_WORK)
        cilk_spawn recursiveBuildTree(node->inner, start, (start + end) / 2);
    else
        recursiveBuildTree(node->inner, start, (start + end) / 2);

    if (end > start)
        recursiveBuildTree(node->outer, (start + end) / 2 + 1, end);
    else
        node->outer = NULL;

    cilk_sync;
}

////////////////////////////////////////////////////////////////////////

vptree *buildvp(double *X, int n, int d)
{
    vptree *root = malloc(sizeof(vptree));
    idArr = malloc(n * sizeof(int));
    distArr = malloc(n * sizeof(double));
    char maxThreadsStr[8];
    Y = X;
    N = n;
    D = d;

    sprintf(maxThreadsStr, "%d", MAX_THREADS);
    __cilkrts_set_param("nworkers", maxThreadsStr);

    for (int i = 0; i < N; i++)
        idArr[i] = i;

    recursiveBuildTree(root, 0, n - 1);

    free(idArr);
    free(distArr);
    return root;
}

///////////////////////////////////////////////////////////

vptree *getInner(vptree *T) { return T->inner; }
vptree *getOuter(vptree *T) { return T->outer; }
double getMD(vptree *T) { return T->md; }
double *getVP(vptree *T) { return T->vp; }
int getIDX(vptree *T) { return T->idx; }
