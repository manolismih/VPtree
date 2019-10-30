#include "details.h"

void quickSelect(int order, double* distArr, int* idArr, int start, int end)
{
    int store=start;
    double pivot = distArr[end];
    for (int i=start; i<end; i++)
        if (distArr[i] < pivot)
        {
            swapDouble(distArr+i, distArr+store);
            swapInt   (idArr+i,   idArr+store);
            store++;
        }
    swapDouble(distArr+end, distArr+store);
    swapInt   (idArr+end,   idArr+store);
    store++;
    if (store < order) quickSelect(order-store-1, distArr, idArr, store+1, end);
    else if (store > order) quickSelect(order, distArr, idArr, start, store);
}
