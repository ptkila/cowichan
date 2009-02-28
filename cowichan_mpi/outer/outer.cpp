/**
 * Outer product matrix
 *
 * \file outer.cpp
 * \author Andrew Borzenko
 * \date 02-27-09
 */

#include "../include/main.h"
#ifdef IS_PARALLEL
  #include "parallel.h"
#else
  #include "serial.h"
#endif

int main(int argc, char* argv[])
{
#ifdef IS_PARALLEL
  mpi::environment env(argc, argv);
  mpi::communicator world;

  printf ("I am process %d\n", world.rank ());
#endif

  pt1D*		ptVec;			/* vector of points */
  real2D*	matrix;			/* matrix to fill */
  real1D*	realVec;		/* vector to fill */

  int    n;    /* size */
  int limit;
  int i;

  srand (222);

  n = MAXEXT;
  limit = 10;

  ptVec = new pt1D[MAXEXT];
  for (i = 0; i < n; i++)
  {
    ptVec[i].x = rand () % limit;
    ptVec[i].y = rand () % limit;
  }

  matrix = new real2D[MAXEXT];

  realVec = new real1D[MAXEXT];

  printf ("point vector:\n");
  print_vector (ptVec, n);

#ifdef IS_PARALLEL
  outer_mpi (world, ptVec, matrix, realVec, n);
#else
  outer (ptVec, matrix, realVec, n);
#endif

  printf ("Inter-point distance matrix:\n");
  print_matrix (matrix, n, n);

  printf ("Origin-to-point distance vector:\n");
  print_vector (realVec, n);

  delete [] ptVec;
  delete [] matrix;
  delete [] realVec;

  return 0;
}
