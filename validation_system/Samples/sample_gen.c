#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RAND 5.0
/*
    Generates random number inside [0.0,MAX_RAND] space
*/
float generateRandFloat(float max)
{
    return (float)rand() / (float)(RAND_MAX / max);
}
int main(int agrc, char *argv[])
{
    /*
        argv[1] : number of points
        argv[2] : number of dimensions
        argv[3] : output name
    */
    int numOfPoints = atoi(argv[1]);
    int numOfDim = atoi(argv[2]);
    FILE *output = fopen(argv[3], "w");

    for (int i = 0; i < numOfPoints; i++)
    {
        for (int j = 0; j < numOfDim - 1; j++)
        {
            fprintf(output, "%f,", generateRandFloat(MAX_RAND));
        }
        fprintf(output, "%f\n", generateRandFloat(MAX_RAND));
    }

    return 0;
}