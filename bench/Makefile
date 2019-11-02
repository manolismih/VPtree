#define the shell to bash
SHELL := /bin/bash

#define the C compiler to use
CC = gcc

#define compile-time flags
CFLAGS = -Wall -O3 -pthread -fopenmp

#define directories containing header files
INCLUDES =-I../inc
######################################

benchmark_sequential: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) benchmark.c bench_util.o vptree_sequential.a -o $@ -lm
	
benchmark_pthreads: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) $(CFLAGS) benchmark.c bench_util.o vptree_pthreads.a -o $@ -lm 
	
benchmark_openmp: benchmark.c bench_util.o
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) benchmark.c bench_util.o vptree_sequential.a -o $@ -lm
	
bench_util.o: bench_util.c	
	$(CC) $(CFLAGS) $(INCLUDES) -c bench_util.c 

clean:
	rm *.o *.a vptree.h benchmark_*
