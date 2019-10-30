//vptree_sequential.c
#include "vptree.h"
#include <stdlib.h>
#include <math.h>

//! Returns the k'th smallest element in dataset X
/*!
    \param array    the given dataset of elements, in a random order
    \param k        the k'th smallest element 
    \param left     index from which searching of requested elem. starts
    \param right    idex from which searching of requested element ends

    \return         The value of k'th smallest element from indices [left ... right]
*/
double qselect(double *array, int left, int right, int k);

//! Groups the array (ranging from indices left to right)
//! into two parts: those less than pivot
//! and thos greater or equal to pivot
/*!
    \param array        the given dataset of elements, in a random order
    \param left         index from which the partitioning starts
    \param right        index from which the partitioning ends
    \param pivotIndex   pivot's index in the array
    \return             The value of k'th smallest element
*/
int partition(double *array, int left, int right, int pivotIndex);

/////////////////////////////////////////////////////////////////////////////

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

        /*


                MAKE A COPY OF distances[] FIRST AND 
                THEN USE qselect()


        */
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

/////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////

vptree *getInner(vptree *T)
{
}

/////////////////////////////////////////////////////////////////////////////

vptree *getOuter(vptree *T)
{
}

/////////////////////////////////////////////////////////////////////////////

double getMD(vptree *T)
{
}

/////////////////////////////////////////////////////////////////////////////

double *getVP(vptree *T)
{
}

/////////////////////////////////////////////////////////////////////////////

int getIDX(vptree *T)
{
}

/////////////////////////////////////////////////////////////////////////////

double qselect(double *array, int left, int right, int k)
{
    if (left == right)
    {
        return array[left];
    }
    //! Assume that pivot is always on the right
    int pivotPos = partition(array, left, right, right);

    if (pivotPos - left == k - 1)
        return array[pivotPos];
    else if (pivotPos - left > k - 1)
        return qselect(array, left, pivotPos - 1, k);
    else
        return qselect(array, pivotPos + 1, right, k - (pivotPos - left + 1));
};

/////////////////////////////////////////////////////////////////////////////

void swap(double *x, double *y)
{
    double temp = *x;
    *x = *y;
    *y = temp;
    return;
};

/////////////////////////////////////////////////////////////////////////////

int partition(double *array, int left, int right, int pivotIndex)
{
    double pivot = array[pivotIndex];
    swap(&array[pivotIndex], &array[right]); //move pitov to end
    int storeIndex = left;
    int i;
    for (i = left; i <= right - 1; i++)
    {
        if (array[i] < pivot)
        {
            swap(&array[storeIndex], &array[i]);
            storeIndex++;
        }
    }
    swap(&array[right], &array[storeIndex]);
    return storeIndex;
}

/////////////////////////////////////////////////////////////////////////////
