# csci-6356-hw3
Completes the tasks outlined in Homework 3 for Dr. Fu's CSCI 6356.
Written by Arnoldo Ramirez at UTRGV, 2019.

Specifically, this program will find:
- The least prime number q where q > n
- The least prime twin (q, q+2) where q > n
- The total number of prime twins (p, p+2) where (p+2) < n

### How to compile
With MPI wrapper installed (preferably on Linux), run:
`mpic++ hw3.cpp`

### How to run
After compiling, you can run something like:

`mpirun -np 5 -stdin 4 a.out 10`
or
`mpirun -np 5 -stdin 4 a.out 5000`
or
`mpirun -np 5 -stdin 4 a.out 1000000`

### Examples
Below you can see three example outputs,
specifically where n=10, n=50, and n=1000000:

![Example](/docs/example.png?raw=true)