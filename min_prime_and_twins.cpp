//        Author:     Arnoldo Ramirez
//        SID:        20254876
//        Course:     CSCI 6356
//        HW:         Homework 3, problems 1-4
//
//        Task:       Finds the least prime number q
//                    where q is (2i+1) (mod 8), and q is greater than m;
//                    also finds the least prime twin greater than m.
//
//   TO BUILD:        mpic++ min_prime_and_twins.cpp
//
//   INTENDED USAGE:  mpirun -np 5 a.out 10
//               or:  mpirun -np 5 a.out 500
//                    where the final argument is used as m,
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

// Finds the smallest prime q as described in the problem,
// i.e. such that q % 8 = (2i+1) where i is process rank.
int prime(int m, int rank)
{
  int q, b = ((2 * rank) + 1), remainder;
  bool found = false;

  //Starts at the next possible prime after m, whether m is odd or even.
  if (m % 2 == 0)
    q = m + 1;
  else
    q = m + 2;

  while(!found)
  {
    if(isPrime(q))
    {
      //Here is the check for condition q % 8 = (2i+1).
      remainder = q % 8;
      if(remainder == b)
        found = true;
    }
    if(!found)
      q = q + 2;
  }

  return q;
}

//Finds the smallest twin prime meeting the condition q % 8 = (2i+1).
int twin(int m, int rank)
{
  int q, b = ((2 * rank) + 1), remainder;
  bool found = false;

  //Starts at the next possible prime after m.
  if (m % 2 == 0)
    q = m + 1;
  else
    q = m + 2;

  while(!found)
  {
    if(isPrime(q))
    {
      //Here is the check for condition q % 8 = (2i+1).
      remainder = q % 8;
      if(remainder == b)
        //Here is the check for twin prime.
        if(isPrime(q + 2))
          found = true;
    }
    if(!found)
      q = q + 2;
  }

  return q;
}


int main (int argc, char *argv[])
{
  int world_size, rank, m, prime_found, twin_found, min_prime_found, min_twin_found;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  //The highest ranked task will collect all the data,
  //e.g. P4 when processors are P0, P1, P2, P3, and P4.
  int LAST = world_size-1;

  // The last argv provided by user is stored as m.
  char* argument1 = argv[1];
  m = std::stoi(argument1);

  //The user provided m is broadcast to all other processes by the highest rank processor,
  //e.g. P4 when processors are P0, P1, P2, P3, and P4.
  MPI_Bcast(&m, 1, MPI_INT, LAST, MPI_COMM_WORLD);

  //All processes except the last will find the minimum primes fitting the problem.
  //The last process will only receive and report the primes.
  if(rank!=LAST)
  {
    prime_found = prime(m, rank);
    printf("Process %d found prime %d\n", rank, prime_found);

    twin_found = twin(m, rank);
    printf("Process %d found twin (%d,%d)\n\n", rank, twin_found, twin_found+2);

    //The primes are sent to the highest rank processor which will determine the minimum.
    MPI_Reduce(&prime_found, &min_prime_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
    MPI_Reduce(&twin_found,&min_twin_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
  }

  //The highest rank processor will receive and report the primes.
  if (rank==LAST)
  {
    printf("m = %s\n\n", argument1);
    MPI_Reduce(&prime_found, &min_prime_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
    MPI_Reduce(&twin_found, &min_twin_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);

    //The minimum prime and twin are printed after being computed.
    printf("Process %d stating that the minimum prime is %d\n", rank, min_prime_found);
    printf("Process %d stating that the minimum twin is (%d,%d)\n", rank,min_twin_found, min_twin_found+2);
  }

  MPI_Finalize();
}
