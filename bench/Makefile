#define the shell to bash
SHELL := /bin/bash

#define the C compiler to use
CC = gcc-7

#define compile-time flags
CFLAGS = -Wall -O3 -pthread -fopenmp  -fcilkplus

#define directories containing header files
INCLUDES =-I../inc
######################################

all: benchmark_cilk benchmark_openmp benchmark_pthreads benchmark_sequential
	find ../ -name "*.a" -delete
	find ../ -name "*.o" -delete

benchmark_sequential: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) $(CFLAGS) benchmark.c bench_util.o vptree_sequential.a -o $@ -lm
	
benchmark_pthreads: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) $(CFLAGS) benchmark.c bench_util.o vptree_pthreads.a -o $@ -lm 
	
benchmark_openmp: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) $(CFLAGS) benchmark.c bench_util.o vptree_openmp.a -o $@ -lm

benchmark_cilk: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) $(CFLAGS) benchmark.c bench_util.o vptree_cilk.a -o $@ -lm 
	
bench_util.o: bench_util.c	
	$(CC) $(CFLAGS) $(INCLUDES) -c bench_util.c 

clean:
	rm *.o *.a vptree.h benchmark_*
