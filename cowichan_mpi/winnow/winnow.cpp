/**
 * Weighted point selection
 *
 * \file winnow.cpp
 * \author Andrew Borzenko
 * \date 02-23-09
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

  int2D* matrix;
  bool2D* mask;
  pt1D* pt;
  int    nr;    /* row size */
  int    nc;    /* column size */
  int limit;
  int npt;
  int i,j;

  //srand ((unsigned int) time (NULL));
  srand (222);

  nr = MAXEXT;
  nc = MAXEXT;
  limit = 10;
  npt = MAXEXT;
  
  matrix = new int2D[MAXEXT];
  for (i = 0; i < nr; i++)
  {
    for (j = 0; j < nc; j++)
    {
      matrix[i][j] = rand () % limit;
    }
  }

  mask = new bool2D[MAXEXT];
  for (i = 0; i < nr; i++)
  {
    for (j = 0; j < nc; j++)
    {
      mask[i][j] = rand () % 2;
    }
  }

  pt = new pt1D[npt];

  printf ("Matrix:\n");
  print_matrix (matrix, nr, nc);

  printf ("Mask:\n");
  print_matrix (mask, nr, nc);

  winnow_mpi (world, matrix, mask, nr, nc, pt, npt); 

  printf ("Points:\n");
  print_vector (pt, nr);

  delete [] matrix;
  delete [] mask;
  delete [] pt;
#else
  int2D* matrix;
  bool2D* mask;
  pt1D* pt;
  int    nr;    /* row size */
  int    nc;    /* column size */
  int limit;
  int npt;
  int i,j;

  //srand ((unsigned int) time (NULL));
  srand (222);

  nr = MAXEXT;
  nc = MAXEXT;
  limit = 10;
  npt = MAXEXT;
  
  matrix = new int2D[MAXEXT];
  for (i = 0; i < nr; i++)
  {
    for (j = 0; j < nc; j++)
    {
      matrix[i][j] = rand () % limit;
    }
  }

  mask = new bool2D[MAXEXT];
  for (i = 0; i < nr; i++)
  {
    for (j = 0; j < nc; j++)
    {
      mask[i][j] = rand () % 2;
    }
  }

  pt = new pt1D[npt];

  printf ("Matrix:\n");
  print_matrix (matrix, nr, nc);

  printf ("Mask:\n");
  print_matrix (mask, nr, nc);

  winnow_mpi (world, matrix, mask, nr, nc, pt, npt); 

  printf ("Points:\n");
  print_vector (pt, nr);

  delete [] matrix;
  delete [] mask;
  delete [] pt;
#endif

	return 0;
}
