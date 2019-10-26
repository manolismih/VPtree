//vptree_sequential.c

#include "vptree.h"
#include <stdlib.h>
#include <math.h>

double qselect(double *array, int k);

vptree *vpt(double *X, int n, int d, int *idx)
{
    vptree *T = (vptree *)malloc(sizeof(vptree));
    double *distances = NULL;
    double *innerX = NULL;
    double *outerX = NULL;
    int *innerIdX;
    int *outerIdX;
    int innerSize, outerSize;
    double sum, medianDistance;
    int i, j, k, l;

    if (n == 0)
        T = NULL;
    else
    {
        //assuming the vantage point is the last one
        T->vp = &X[(n - 1) * d];
        T->idx = idx[n - 1];
        distances = malloc((n - 1) * sizeof(double));
        for (i = 0; i < n - 1; i++) //for each point except the last
        {
            //calculate the distance to vantage point
            sum = 0.0;
            for (j = 0; j < d; j++)
            {
                sum += pow(X[i * d + j] - X[(n - 1) * d + j], 2);
            }
            distances[i] = sqrt(sum);
        }

        //find the median distance with quickselect
        medianDistance = qselect(distances, n / 2);
        T->md = medianDistance;

        /*this loop counts how many points there are 
         *for inner vptree
        */
        i = 0;
        for (j = 0; j < n - 1; j++)
        {
            if (distances[j] <= medianDistance)
                i++;
        }
        innerSize = i;
        outerSize = n - i;

        innerX = (double *)malloc(d * innerSize * sizeof(int));
        innerIdX = (int *)malloc(innerSize * sizeof(double));

        outerX = (double *)malloc(d * outerSize * sizeof(int));
        outerIdX = (int *)malloc(outerSize * sizeof(double));

        //fill the subtree X and IdX array
        i = 0;
        j = 0;
        for (k = 0; k < n; k++)
        {
            if (distances[k] <= medianDistance)
            {
                innerIdX[i] = idx[k];
                for (l = 0; l < d; l++)
                {
                    innerX[i * d + l] = X[k * d + l];
                }
                i++;
            }
            else
            {
                outerIdX[j] = idx[k];
                for (l = 0; l < d; l++)
                {
                    outerX[j * d + l] = X[k * d + l];
                }
                j++;
            }
        }

        //recursion
        T->inner = vpt(innerX, innerSize, d, innerIdX);
        T->outer = vpt(outerX, outerSize, d, outerIdX);

        //memory deallocation
        free(distances);
        free(innerX);
        free(outerX);
        free(innerIdX);
        free(outerIdX);
    }

    return T;
};

vptree *buildvp(double *X, int n, int d)
{
    vptree *T = (vptree *)malloc(sizeof(vptree));
    // creates an ID array
    // to keep track of element's ID
    int *idx = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        idx[i] = i;
    }

    T = vpt(X, n, d, idx);

    //memory clean-up
    free(idx);

    return T;
}

vptree *getInner(vptree *T)
{
}

vptree *getOuter(vptree *T)
{
}

double getMD(vptree *T)
{
}

double *getVP(vptree *T)
{
}

int getIDX(vptree *T)
{
}