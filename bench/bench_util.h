#ifndef BENCH_UTIL_H
#define BENCH_UTIL_H

#include <stdio.h>
#include "vptree.h"

double *csvread(FILE *csvFile, int N, int d);

void sampleGen(int N, int d, double X[N][d]);

int getN(char *fileName);

int getD(char *fileName);

void preOrderTraversal(vptree *root);

#endif
