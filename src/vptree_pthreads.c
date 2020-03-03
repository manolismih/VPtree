#include "vptree.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN_WORK_PER_THREAD 10000
//global active thread counter
long MAX_THREADS;
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

inline double sqr(double x) {return x*x;}
void distCalc(double *vp, int start, int end)
{
    double(*dataArr)[D] = (double(*)[D])Y;
    for (int i=start; i<=end; i++)
        distArr[i] = sqr(vp[0] - dataArr[idArr[i]][0]);
    for (int i=start; i<=end; i++)
        for (int j=1; j<D; j++)
            distArr[i] += sqr(vp[j] - dataArr[idArr[i]][j]);
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
void quickSelect(int kpos, double* distArr, int* idArr, int start, int end)
{
    int store=start;
    double pivot=distArr[end];
    for (int i=start; i<=end; i++)
        if (distArr[i] <= pivot)
        {
            swapDouble(distArr+i, distArr+store);
            swapInt   (idArr+i,   idArr+store);
            store++;
        }        
    store--;
    if (store == kpos) return;
    else if (store < kpos) quickSelect(kpos, distArr, idArr, store+1, end);
    else quickSelect(kpos, distArr, idArr, start, store-1);
}

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

    int freeThreads = MAX_THREADS-activeThreads;
    if (freeThreads<=0 || (end-start+1)*D < MIN_WORK_PER_THREAD)
        distCalc(node->vp,start,end); //sequential calculation
    else
    {
        updateThreadCounter(+freeThreads);
        int workPerThread = (end-start+1) / (freeThreads+1);
        //array of thread objects for distArr calculation
        pthread_t threadObj[freeThreads];
        //we need an array of different args, each one for each thread
        threadArgs Args[freeThreads];

        for (int id=0; id<freeThreads; id++)
        {
            Args[id].start = start +id*workPerThread;
            Args[id].end = Args[id].start+workPerThread-1;
            Args[id].node = node;
            pthread_create(threadObj+id, NULL, threadDistCalc, (void*)(Args+id));
        }
        //current thread does it's share + the leftovers till end
        distCalc(node->vp, start +freeThreads*workPerThread, end);

        for (int id=0; id<freeThreads; id++)
            pthread_join(threadObj[id], NULL);
        updateThreadCounter(-freeThreads);
    }

    quickSelect((start+end)/2, distArr, idArr, start, end);
    // now idArr[start .. (start+end)/2] contains the indexes
    // for the points which belong inside the radius (inner)

    node->md = sqrt(distArr[ (start+end)/2 ]);
    node->inner = malloc(sizeof(vptree));
    node->outer = malloc(sizeof(vptree));

    if (activeThreads<MAX_THREADS && (end-start+1)*D/2 >= MIN_WORK_PER_THREAD)
    {
        updateThreadCounter(+1);
        //create the arguments to pass
        pthread_t threadInner;
        threadArgs innerArgs;
        innerArgs.node = node->inner;
        innerArgs.start = start;
        innerArgs.end = (start+end)/2;

        pthread_create(&threadInner, NULL, threadRecBuildTree, (void*)&innerArgs);
        recursiveBuildTree(node->outer, (start+end)/2 +1, end);
        pthread_join(threadInner, NULL);
        updateThreadCounter(-1);
    }
    else {
        recursiveBuildTree(node->inner, start, (start+end)/2);
        if (end > start) recursiveBuildTree(node->outer, (start+end)/2 +1, end);
        else node->outer = NULL;
    }
}

////////////////////////////////////////////////////////////////////////

vptree *buildvp(double *X, int n, int d)
{
    vptree *root = malloc( sizeof(vptree) );
    idArr        = malloc( n*sizeof(int) );
    distArr      = malloc( n*sizeof(double) );
    Y=X, N=n, D=d;
    pthread_mutex_init(&mutexCounter, NULL);
    MAX_THREADS = sysconf(_SC_NPROCESSORS_ONLN);
    for (int i=0; i<N; i++) idArr[i] = i;

    recursiveBuildTree(root, 0, n-1);
    
    pthread_mutex_destroy(&mutexCounter);
    free(idArr);
    free(distArr);
    return root;
}

////////////////////////////////////////////////////////////////////////

void *threadRecBuildTree(void* arguments)
{
    threadArgs *args = (threadArgs*)arguments;
    recursiveBuildTree(args->node, args->start, args->end);
    pthread_exit( (void*)0 );
    return (void*)0;
}

////////////////////////////////////////////////////////////////////////

void *threadDistCalc(void* arguments)
{
    threadArgs* args = (threadArgs*)arguments;
    distCalc(args->node->vp, args->start, args->end);
    pthread_exit( (void*)0 );
    return (void*)0;
}

///////////////////////////////////////////////////////////

vptree *getInner(vptree *T) { return T->inner; }
vptree *getOuter(vptree *T) { return T->outer; }
double getMD(vptree *T)     { return T->md;    }
double *getVP(vptree *T)    { return T->vp;    }
int getIDX(vptree *T)       { return T->idx;   }
