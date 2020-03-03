#define the shell to bash
SHELL := /bin/bash

#define the C compiler to use
CC = gcc-7

#define compile-time flags
CFLAGS = -Wall -O3 -pthread -fopenmp  -fcilkplus

#define includes
INCL = -I../inc

#define directories containing header files
INCLUDES =-I ../inc
######################################

benchmarks: benchmark_cilk benchmark_openmp benchmark_pthreads benchmark_sequential

benchmark_sequential: benchmark.c bench_util.c ../src/vptree_sequential.c
	$(CC) $(CFLAGS) $(INCL) $^ -o $@ -lm
	
benchmark_pthreads: benchmark.c bench_util.c ../src/vptree_pthreads.c
	$(CC) $(CFLAGS) $(INCL) $^ -o $@ -lm
	
benchmark_openmp: benchmark.c bench_util.c ../src/vptree_openmp.c
	$(CC) $(CFLAGS) $(INCL) $^ -o $@ -lm

benchmark_cilk: benchmark.c bench_util.c ../src/vptree_cilk.c
	$(CC) $(CFLAGS) $(INCL) $^ -o $@ -lm

clean:
	find ../ -name "benchmark_*" -delete
