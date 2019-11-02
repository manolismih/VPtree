#include "bench_util.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_CHARS 1024
#define MAX_FP_DIGITS 30
#define MAX_RAND_FLOAT 5.0
double *csvread(FILE *csvFile, int N, int d)
{
    double **X = malloc(N * sizeof(double *));
    for (int i = 0; i < N; i++)
    {
        X[i] = malloc(d * sizeof(double));
    }
    const char delim[2] = ",";
    char *buffer = malloc(MAX_LINE_CHARS * sizeof(char));
    char *token = malloc(MAX_FP_DIGITS * sizeof(char));

    int i = 0;
    int j = 0;
    while (fgets(buffer, MAX_LINE_CHARS, csvFile) != NULL && i < N)
    {
        buffer[strlen(buffer) - 2] = '\0';
        token = strtok(buffer, delim);
        j = 0;
        while (token != NULL && j < d)
        {
            X[i][j] = atof(token);
            token = strtok(NULL, delim);
            j++;
        }
        i++;
    }

    return (double *)X;
}

void sampleGen(int N, int d, double X[N][d])
{
    srand(time(NULL));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < d; j++)
        {
            X[i][j] = (double)rand()/(double)(RAND_MAX/MAX_RAND_FLOAT);
        }
    }
}
void preOrderTraversal(vptree *root)
{
    if (root != NULL)
    {
        printf("%d ", root->idx);
        preOrderTraversal(root->inner);
        preOrderTraversal(root->outer);
    }
}