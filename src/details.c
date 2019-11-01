#include "details.h"

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
