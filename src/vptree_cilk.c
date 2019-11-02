#include "vptree.h"
#include "details.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include <stdlib.h>
#include <math.h>

#define ELEMS_PER_THREAD 3

void distCalc(
    vptree *node,
    int start, int end,
    int n, int d,
    int idArr[n],
    double distArr[n],
    double dataArr[n][d])
{
    for (int i = start; i <= end; i++)
        distArr[i] = sqr(node->vp[0] - dataArr[idArr[i]][0]);
    for (int i = start; i <= end; i++)
        for (int j = 1; j < d; j++)
            distArr[i] += sqr(node->vp[j] - dataArr[idArr[i]][j]);
}

void recursiveBuildTree(
    vptree *node,
    int start, int end,
    int n, int d,
    int idArr[n],
    double distArr[n],
    double dataArr[n][d])
{
    int numOfThreads;
    int threadStart;
    int threadEnd;

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

    numOfThreads = (end - start + 1) / ELEMS_PER_THREAD;
    if (numOfThreads > 1)
    {
        for (int i = 0; i < numOfThreads; i++)
        {
            threadStart = start + i * ELEMS_PER_THREAD;
            threadEnd = start + (i + 1) * ELEMS_PER_THREAD - 1;

            if (threadEnd + ELEMS_PER_THREAD > end)
                threadEnd = end;

            cilk_spawn distCalc(node, threadStart, threadEnd, n, d, idArr, distArr, dataArr);
        }
        cilk_sync;
    }
    else
        distCalc(node, start, end, n, d, idArr, distArr, dataArr);

    quickSelect((start + end) / 2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = sqrt(distArr[(start + end) / 2]);
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));

    cilk_spawn recursiveBuildTree(node->inner, start, (start + end) / 2, n, d, idArr, distArr, dataArr);

    if (end > start)
        recursiveBuildTree(node->outer, (start + end) / 2 + 1, end, n, d, idArr, distArr, dataArr);
    else
        node->outer = NULL;

    cilk_sync;
}

/////////////////////////////////////////////////////////////////////////////

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
/////////////////////////////////////////////////////////////////////////////
vptree *getInner(vptree *T) { return T->inner; }
vptree *getOuter(vptree *T) { return T->outer; }
double getMD(vptree *T) { return T->md; }
double *getVP(vptree *T) { return T->vp; }
int getIDX(vptree *T) { return T->idx; }
