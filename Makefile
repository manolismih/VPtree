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
CC = gcc-7

#define compile-time flags
CFLAGS = -Wall -fcilkplus -fopenmp -03

#define directories containing header files
INCLUDES = -I./inc

#define the source file for the library
SRC = vptree

#define the different executables
VERSIONS = sequential pthreads openmp Cilk

#define the executable file name
MAIN = main

#call everytime
all: $(addprefix $(MAIN)_, $(TYPES))

lib: $(addsuffix .a, $(addprefix $(SRC)_, $(TYPES)))

$(MAIN)_%: $(MAIN).c $(SRC)_%.a
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

.o.a:
	ar rcs $@ $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	$(RM) *.o *~ $(addprefix $(MAIN)_, $(TYPES)) fib_*.a