# ################################################
#
#					C Makefile
#		PDS Exercise 1: Vantage-Point Tree Build
#	  Versions: Sequential, Pthreads, openMP, Cilk
#
#			Author: Panagiotis Karelis (9099)
#           Author: Emmanouil Michalainas (9070) 
# ################################################
#
# 'make' build executable file 'main'
# 'make lib' build the libraries .a
# 'make clean' removes all .o and executables
#

#define the shell to bash
SHELL := /bin/bash

#define the C compiler to use
CC = gcc-7

#define compile-time flags
CFLAGS = -Wall -O3 -pthread -fopenmp -fcilkplus

#define directories containing header files
INCLUDES = -I ./inc

#define objects
SL = vptree_sequential.a vptree_pthreads.a vptree_cilk.a vptree_openmp.a

########################################################################

lib: $(SL)

%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o lib/$@

%.a: %.o
	ar rcs lib/$@ lib/$<
	rm lib/$<

clean:
	find . -name "benchmark_*" -delete
	find . -name "*.a" -delete