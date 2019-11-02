# define the shell to bash
SHELL := /bin/bash

# define the C/C++ compiler to use,default here is clang
CC = gcc-7

test_sequential:
	cd ..; make lib; 
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) tester.c vptree_sequential.a -o $@ -lm
	./test_sequential
	rm test_sequential *.a *.h ../lib/*.a

test_pthreads:
	cd ..; make lib;
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) -pthread tester.c vptree_pthreads.a -o $@ -lm
	./test_pthreads
	rm test_pthreads *.a *.h ../lib/*.a

test_openmp:
	cd ..; make lib;
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) -fopenmp tester.c vptree_openmp.a -o $@ -lm
	./test_openmp
	rm test_openmp *.a *.h ../lib/*.a

test_cilk:
	cd ../; make lib;
	cp ../lib/*.a ../inc/vptree.h ./;
	$(CC) -fcilkplus tester.c vptree_cilk.a -o $@ -lm
	./test_cilk
	rm test_cilk *.a *.h ../lib/*.a
