#ifndef DETAILS_H
#define DETAILS_H

inline double sqr(double x) {return x*x;}
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
void quickSelect(int kpos, double* distArr, int* idArr, int start, int end);

#endif
