#ifndef DETAILS_H
#define DETAILS_H

inline double sqr(double x) {return x*x;}
inline void swapDouble(double* a, double* b)
{
    double temp = *a;
    *b = *a;
    *a = temp;
}
inline void swapInt(int* a, int* b)
{
    int temp = *a;
    *b = *a;
    *a = temp;
}
void quickSelect(int order, double* distArr, int* idArr, int start, int end);

#endif
