# ################################################
#
#					C Makefile
#		PDS Exercise 1: Vantage-Point Tree Build
#	  Versions: Sequential, Pthreads, openMP, Cilk
#
#			Author: Panagiotis Karelis (9099) 
# ################################################
#
# 'make' build executable file 'main'
# 'make lib' build the libraries .a
# 'make clean' removes all .o and executables
#

#define the shell to bash
SHELL := /bin/bash

#define the C compiler to use
CC = gcc

#define compile-time flags
CFLAGS = -Wall -O3

#define directories containing header files
INCLUDES = -I ./inc

########################################################################

test_manos: test_manos.c lib
	$(CC) $(CFLAGS) $(INCLUDES) test_manos.c lib/vptree_sequential.a -o test_manos -lm
	./test_manos
	rm test_manos

lib: vptree_cilk.o vptree_openmp.o vptree_pthreads.o vptree_sequential.o
	ar rcs lib/vptree_cilk.a lib/vptree_cilk.o
	ar rcs lib/vptree_openmp.a lib/vptree_openmp.o
	ar rcs lib/vptree_pthreads.a lib/vptree_pthreads.o
	ar rcs lib/vptree_sequential.a lib/vptree_sequential.o lib/details.o
	rm lib/vptree_cilk.o lib/vptree_sequential.o lib/vptree_openmp.o lib/vptree_pthreads.o lib/details.o
	
vptree_cilk.o : src/vptree_cilk.c
	$(CC) $(CFLAGS) $(INCLUDES) -c src/vptree_cilk.c -o lib/vptree_cilk.o
	
vptree_sequential.o : src/vptree_sequential.c src/details.c
	$(CC) $(CFLAGS) $(INCLUDES) -c src/vptree_sequential.c -o lib/vptree_sequential.o
	$(CC) $(CFLAGS) $(INCLUDES) -c src/details.c -o lib/details.o
	
vptree_pthreads.o : src/vptree_pthreads.c
	$(CC) $(CFLAGS) $(INCLUDES) -c src/vptree_pthreads.c -o lib/vptree_pthreads.o
	
vptree_openmp.o : src/vptree_openmp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c src/vptree_openmp.c -o lib/vptree_openmp.o
