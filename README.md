# Vantage-Point Tree (vp-tree) construction in pthreads, cilk and openmp
_Exercise for Parallel & Distributed Systems course, school of Electrical and Computer Engineering, Aristotle University of Thessaloniki._

**The Vantage-Point** Tree is a data structure used to perform the _k_ nearest neighbour search (**_kNN_**) of a query set inside a corpus set. The vptree is composed by the points of corpus set. This structure is introduced in the publication:

> _Peter N Yianilos, Data structures and algorithms for nearest neighbor search in general metric spaces, In
Fourth annual ACM-SIAM symposium on Discrete algorithms. Society for Industrial and Applied Mathematics Philadelphia, volume 93, pages 311–321, 1993._

The algorithm is based on a very simple idea: select a point as the vantage point, compute the distance of every point
to the vantage point and split the points according to the median distance, then recurse for left (inner) and right (outer) sub-array. For the splitting, we used _QuickSelect_ algorithm.

For the above construction process, there are two things to do in parallel:  
* compute the distances in parallel
* compute the inner and outer set in parallel

## Benchmarks

We ran benchmarks to compare times between different shared memory parallel implementations and sequential version. We ran the benchmarks on a dual-core processor (3GHz) with 4GB of DDR2 RAM, and the results show that we almost achieved the theoretical maximum acceleration (which is _x2_) in _pthreads_ and _cilk_. Times are reported in seconds. (N: # of points, D: # of dimensions).

![Benchmarks table](https://raw.githubusercontent.com/karelispanagiotis/PDS_Exercise_1/master/vptree_benchmarks.jpeg)

A full report is available in greek [here](https://github.com/karelispanagiotis/PDS_Exercise_1/raw/master/report.pdf).

## How to use
By running the following command:

>$ make lib  
    
you can use the header file _vptree.h_, located inside _inc_/  folder along with static library files _vptree_version.a_, located inside _lib_/ folder, for your own project.   
Version can be any of the following: _cilk_, _pthreads_, _openmp_, _sequential_.

You can also test benchmark times for your own system by doing

>$ cd bench  
 $ make benchmarks  
 $ ./benchmark_version numOfPoints dimensions

where 
* version is any of the following: _cilk_, _pthreads_, _openmp_, _sequential_.
* numOfPoints is a positive integer
* dimensions is also a positive integer.

For example, in order to see the execution time of building a vptree of 1000000 points and 5 dimensionss with pthreads, one whould write:
> $ ./benchmark_pthreads 1000000 5