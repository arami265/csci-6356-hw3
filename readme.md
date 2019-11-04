# csci-6356-hw3

Written by Arnoldo Ramirez at UTRGV, 2019.

This project contains two programs:

- `min_prime_and_twins.cpp` for problems 1-4
- `twin_count.cpp` for problem 5

### How to build
With MPI wrapper installed (preferably on Linux), run

`mpic++ min_prime_and_twins.cpp` or

`mpic++ twin_count.cpp`.

### How to run
For `min_prime_and_twins.cpp` run something like: `mpirun -np 5 a.out 10`.

For `twin_count.cpp` run something like: `mpirun -np 5 a.out 1000000`.