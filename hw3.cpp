//        Author:     Arnoldo Ramirez
//        SID:        20254876
//        Course:     CSCI 6356
//        HW:         Homework 3
//
//        Task:       Find the least prime number q where q > n,
//                    the least prime twin (q, q+2) where q > n,
//                    and the total number of prime twins (p, p+2)
//                    where (p+2) < n.
//
//   TO BUILD:        mpic++ hw3.cpp
//
//   INTENDED USAGE:  mpirun -np 5 -stdin 4 a.out 10
//               or:  mpirun -np 5 -stdin 4 a.out 500000
//                    where the final argument is used as n,
//                    and 5 processors are used.
//
//   Github:          https://github.com/arami265/csci-6356-hw3

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

//Basic function to check if an integer n is prime.
bool isPrime(int n)
{
  if(n < 2) return false;

  if(n == 2) return true;

  if(n % 2 == 0) return false;

  for(int i = 3; (i*i) <= n; i += 2)
      if(n % i == 0) return false;

  return true;
}

//Finds the smallest prime q where q % 8 = (2i+1) and i is process rank,
//and q > n.
int prime(int n, int rank)
{
  int q, b = ((2 * rank) + 1), remainder;
  bool found = false;

  //Starts at the next possible prime after n, whether n is odd or even.
  if (n % 2 == 0)
    q = n + 1;
  else
    q = n + 2;

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
      //Iterates by 2 to skip even numbers.
      q = q + 2;
  }

  return q;
}

//Finds the smallest twin prime meeting the condition q % 8 = (2i+1).
int twin(int n, int rank)
{
  int q, b = ((2 * rank) + 1), remainder;
  bool found = false;

  //Starts at the next possible prime after n, whether n is odd or even.
  if (n % 2 == 0)
    q = n + 1;
  else
    q = n + 2;
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
      //Iterates by 2 to skip even numbers
      q = q + 2;
  }

  //Returns the lower of the pair, as this is enough information to reproduce
  //the full pair.
  return q;
}

//Counts pairs of twin primes (p, p+2) where p % 8 = (2i+1) and i
//is process rank, and both p and p+2 are prime.
int countTwin(int n, int rank)
{
  int b = ((2 * rank) + 1), remainder, twin_count = 0;

  //Bounded to fit the constraint of (p+2) < n;
  //iterates by 2 to skip even numbers.
  for(int p = 1; (p+2) < n; p = p + 2)
  {
    if(isPrime(p))
    {
      //Here is the check for condition p % 8 = (2i+1).
      remainder = p % 8;
      if(remainder == b)
        //Here is the check for twin prime.
        if(isPrime(p + 2))
          //If a twin is found, 2 is added to the count.
          twin_count = twin_count + 2;
    }
  }

  return twin_count;
}


int main (int argc, char *argv[])
{
  int world_size, rank, n,
  prime_found, min_prime_found,
  twin_found, min_twin_found,
  twin_count = 0, total_twin_count = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  //The highest ranked task will collect all the data,
  //e.g. P4 when processors are P0, P1, P2, P3, and P4.
  int LAST = world_size-1;

  // The last argv provided by user is stored as n.
  std::string argument1 = argv[1];
  n = std::stoi(argument1);

  //The user provided n is broadcast to all other processes by the highest rank processor,
  //e.g. P4 when processors are P0, P1, P2, P3, and P4.
  MPI_Bcast(&n, 1, MPI_INT, LAST, MPI_COMM_WORLD);

  //All processes except the last will find the primes fitting the problem.
  //The last process will only receive and report the findings.
  if(rank != LAST)
  {
    prime_found = prime(n, rank);
    //printf("Process %d found prime %d\n", rank, prime_found);

    twin_found = twin(n, rank);
    //printf("Process %d found twin (%d,%d)\n\n", rank, twin_found, twin_found+2);

    twin_count = countTwin(n, rank);
    //printf("Process %d counted %d twins.\n\n", rank, twin_count);

    //The primes are sent to the highest rank processor which will determine the minimums
    //and sum the total of the twin count.
    // int MPI_Gatherv(void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm MPI_COMM_WORLD);
    MPI_Reduce(&prime_found, &min_prime_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
    MPI_Reduce(&twin_found,&min_twin_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
    MPI_Reduce(&twin_count, &total_twin_count, 1, MPI_INT, MPI_SUM, LAST, MPI_COMM_WORLD);
  }

  //The highest rank processor will receive and report the primes.
  if (rank==LAST)
  {
    //Quick way to ensure that P4 finds the true minimums; ideally, a combination
    //of various MPI functions can solve this more efficiently.
    prime_found = n*100, min_prime_found = n*100,
    twin_found = n*100, min_twin_found = n*100;

    MPI_Reduce(&prime_found, &min_prime_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
    MPI_Reduce(&twin_found, &min_twin_found, 1, MPI_INT, MPI_MIN, LAST, MPI_COMM_WORLD);
    MPI_Reduce(&twin_count, &total_twin_count, 1, MPI_INT, MPI_SUM, LAST, MPI_COMM_WORLD);

    //The results are printed after being computed.
    printf("n = %s\n\n", argument1.c_str());
    printf("Minimum prime > n: %d\n\n", min_prime_found);
    printf("Minimum prime twin > n: (%d, %d)\n\n", min_twin_found, min_twin_found+2);
    printf("Total sum of prime twins < n: %d\n\n", total_twin_count);
  }

  MPI_Finalize();
}
