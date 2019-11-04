//        Author:     Arnoldo Ramirez
//        SID:        20254876
//        Course:     CSCI 6356
//        HW:         Homework 3, problem 5
//
//        Task:       Counts the number of twin numbers(p, p+2)
//                    where p=2i+1(mod 8), and p+2<n.
//
//   TO BUILD:        mpic++ twin_count.cpp
//
//   INTENDED USAGE:  mpirun -np 5 a.out 100
//               or:  mpirun -np 5 a.out 1000000
//                    where the final argument is used as n,
//                    and 5 processors are used.
//
//   Github:          https://github.com/arami265/csci-6356-hw3

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

//Function to check if n is prime
bool isPrime(int n)
{
  if (n <= 1)
      return false;
  if (n <= 3)
      return true;

  if (n % 2 == 0 || n % 3 == 0)
      return false;

  for (int i = 5; i * i <= n; i = i + 6)
      if (n % i == 0 || n % (i + 2) == 0)
          return false;

  return true;
}

//Counts twin primes q where q % 8 = (2i+1) where i is process rank,
//and both q and q + 2 are prime.
int countTwin(int n, int rank)
{
  int b = ((2 * rank) + 1), remainder, twin_count = 0;

  for(int q = 1; q < (n - 2); q++)
  {
    if(isPrime(q))
    {
      //Here is the check for condition q % 8 = (2i+1).
      remainder = q % 8;
      if(remainder == b)
        //Here is the check for twin prime.
        if(isPrime(q + 2))
          //If a twin is found, 1 is added to the count.
          twin_count++;
    }
  }

  return twin_count;
}


int main (int argc, char *argv[])
{
  int world_size, rank, n, twin_count = 0, total_twin_count = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  //The highest ranked task will collect all the data,
  //e.g. P4 when processors are P0, P1, P2, P3, and P4.
  int LAST = world_size - 1;

  // The last argv is used as n.
  char* argument1 = argv[1];
  n = std::stoi(argument1);

  //The user provided n is broadcast to all other processes by the highest rank processor,
  //e.g. P4 when processors are P0, P1, P2, P3, and P4.
  MPI_Bcast(&n, 1, MPI_INT, LAST, MPI_COMM_WORLD);

  //All processes except the last will find the minimum prime fitting the problem.
  //The last process will only receive and report the primes.
  if(rank != LAST)
  {
    twin_count = countTwin(n, rank);
    printf("Process %d counted %d twins.\n\n", rank, twin_count);

    //The found primes are sent to the highest rank processor.
    MPI_Reduce(&twin_count, &total_twin_count, 1, MPI_INT, MPI_SUM, LAST, MPI_COMM_WORLD);
  }

  //The highest rank processor will receive and report the primes.
  if (rank == LAST)
  {
    printf("n = %s\n\n", argument1);
    MPI_Reduce(&twin_count, &total_twin_count, 1, MPI_INT, MPI_SUM, LAST, MPI_COMM_WORLD);

    //The minimum prime and twin are printed after being computed.
    printf("Process %d stating that there are %d total twin primes less than %d.\n", rank,total_twin_count, n);
  }

  MPI_Finalize();
}
