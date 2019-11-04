#include "vptree.h"
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////
/*********************** THREADING GENERAL ****************************/
#define MIN_WORK_PER_THREAD 1000000
#define MAX_THREADS 2
//global active thread counter
int activeThreads=1;
pthread_mutex_t mutexCounter;

inline void updateThreadCounter(int x)
{
    pthread_mutex_lock(&mutexCounter);
    activeThreads += x;
    pthread_mutex_unlock(&mutexCounter);
}

////////////////////////////////////////////////////////////////////////
/*********************** STORAGE & GLOBALS ****************************/

int*    idArr;
double* distArr;
double* Y; // hard(deep) copy of the input X array 
int     N, D;

////////////////////////////////////////////////////////////////////////
/*********************** FORWARD DECLARATIONS *************************/

void parallelDistCalc(double* vp, int start, int end);
void parallelRecursiveBuildTree(double* X, vptree* node, int start, int end);

////////////////////////////////////////////////////////////////////////
/*********************** SEQUENTIAL FUNCTIONS *************************/

inline double sqr(double x) {return x*x;}
void distCalc(double *vp, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])Y;
    for (int i=start; i<=end; i++)
        distArr[i] = sqr(vp[0] - dataArr[i][0]);
    for (int i=start; i<=end; i++)
        for (int j=1; j<D; j++)
            distArr[i] += sqr(vp[j] - dataArr[i][j]);
};

////////////////////////////////////////////////////////////////////////

inline void swapDouble(double* a, double* b)
{
    double temp = *a;
    *a = *b;
    *b = temp;
}
inline void swapInt(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void quickSelect(int kpos, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])Y;
    int store=start;
    double pivot=distArr[end];
    for (int i=start; i<=end; i++)
        if (distArr[i] <= pivot)
        {
            swapDouble(distArr+i, distArr+store);
            swapInt   (idArr+i,   idArr+store);
            memcpy    (dataArr+N,     dataArr+i,     D*sizeof(double));
            memcpy    (dataArr+i,     dataArr+store, D*sizeof(double));
            memcpy    (dataArr+store, dataArr+N,     D*sizeof(double));
            store++;
        }        
    store--;
    if (store == kpos) return;
    else if (store < kpos) quickSelect(kpos, store+1, end);
    else quickSelect(kpos, start, store-1);
}

////////////////////////////////////////////////////////////////////////

void recursiveBuildTree(double* X, vptree* node, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])X;
    /* In this function we need direct access to X because the copy Y
     * will be freed eventually. This function will not modify X but
     * will assign a pointer to the coordinates of the vantage point */
    
    //consider dataArr[end] as vantage point
    node->idx = idArr[end];
    //caution! dataArr=X and therefore not swapped
    node->vp  = dataArr[ idArr[end] ]; 
    if (start==end)
    {
        node->inner = node->outer = NULL;
        node->md = 0.0;
        return;
    }
    end--; //end is the vantage point, we're not dealing with it again

    
    if (activeThreads>=MAX_THREADS || (end-start+1)*D < 2*MIN_WORK_PER_THREAD)
        distCalc(node->vp, start, end); //sequential calculation
    else
        parallelDistCalc(node->vp, start, end);    

    quickSelect((start+end)/2, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md  = sqrt(distArr[ (start+end)/2 ]);
    node->inner = malloc( sizeof(vptree) );
    node->outer = malloc( sizeof(vptree) );

    if (activeThreads<MAX_THREADS && (end-start+1)*D/2 >= MIN_WORK_PER_THREAD)
        parallelRecursiveBuildTree(X, node, start, end);
    else {
        recursiveBuildTree(X, node->inner, start, (start+end)/2);
        if (end>start) recursiveBuildTree(X, node->outer, (start+end)/2 +1, end);
        else node->outer = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
/*********************** PTHREAD FUNCTIONS ****************************/

typedef struct DistCalcArgs{
    double* vp;
    int start, end;
} DistCalcArgs;
void *threadDistCalc(void* arguments)
{
    DistCalcArgs* args = (DistCalcArgs*)arguments;
    //~ printf("Thread [%d %d]\n",args->start, args->end);
    distCalc(args->vp, args->start, args->end);
    pthread_exit( (void*)0 );
    return (void*)0;
}

////////////////////////////////////////////////////////////////////////

typedef struct RecBuildTreeArgs{
    double* X;
    vptree* node;
    int start, end;
} RecBuildTreeArgs;
void *threadRecBuildTree(void* arguments)
{
    RecBuildTreeArgs *args = (RecBuildTreeArgs*)arguments;
    recursiveBuildTree(args->X, args->node, args->start, args->end);
    pthread_exit( (void*)0 );
    return (void*)0;
}

////////////////////////////////////////////////////////////////////////
/*********************** PARALLEL FUNCTIONS ***************************/

void parallelDistCalc(double* vp, int start, int end)
{
    int freeThreads = MAX_THREADS-activeThreads;
    int useThreads = (end-start+1)*D/MIN_WORK_PER_THREAD -1;
    if (useThreads > freeThreads) useThreads = freeThreads;
    updateThreadCounter(+useThreads);
    int workPerThread = (end-start+1) / (useThreads+1);
    //array of thread objects for distArr calculation
    pthread_t threadObj[useThreads];
    //we need an array of different args, each one for each thread
    DistCalcArgs Args[useThreads];

    for (int id=0; id<useThreads; id++)
    {
        Args[id].start = start +id*workPerThread;
        Args[id].end = Args[id].start+workPerThread-1;
        Args[id].vp = vp;
        pthread_create(threadObj+id, NULL, threadDistCalc, (void*)(Args+id));
    }
    //current thread does it's share + the leftovers till end
    distCalc(vp, start +useThreads*workPerThread, end);

    for (int id=0; id<useThreads; id++)
        pthread_join(threadObj[id], NULL);
    updateThreadCounter(-useThreads);
}

////////////////////////////////////////////////////////////////////////

void parallelRecursiveBuildTree(double* X, vptree* node, int start, int end)
{
    updateThreadCounter(+1);
    //create the arguments to pass
    pthread_t threadObj;
    RecBuildTreeArgs Args;
    Args.X = X;
    Args.node = node->inner;
    Args.start = start;
    Args.end = (start+end)/2;

    pthread_create(&threadObj, NULL, threadRecBuildTree, (void*)&Args);
    recursiveBuildTree(X, node->outer, (start+end)/2 +1, end);
    pthread_join(threadObj, NULL);
    updateThreadCounter(-1);
}

////////////////////////////////////////////////////////////////////////
/************************** ENTRY POINT *******************************/

vptree *buildvp(double *X, int n, int d)
{
    pthread_mutex_init(&mutexCounter, NULL);
    vptree *root = malloc( sizeof(vptree) );
    idArr        = malloc( n*sizeof(int) );
    distArr      = malloc( n*sizeof(double) );
    Y            = malloc( (n+1)*d*sizeof(double));
    //Y[n] will be used for swapping in quickSelect
    
    N=n, D=d;
    for (int i=0; i<N; i++) idArr[i] = i;
    memcpy(Y, X, n*d*sizeof(double));
            
    recursiveBuildTree(X, root, 0, n-1);
    
    pthread_mutex_destroy(&mutexCounter);
    free(Y);
    free(distArr);
    free(idArr);
    return root;
}

////////////////////////////////////////////////////////////////////////

vptree* getInner(vptree* T){    return T->inner;}
vptree* getOuter(vptree* T){    return T->outer;}
double getMD(vptree* T)    {    return T->md;   }  
double* getVP(vptree* T)   {    return T->vp;   }
int getIDX(vptree* T)      {    return T->idx;  }

