/**
 * Chained cowichan implementations
 *
 * \file chain.cpp
 * \author Andrew Borzenko
 * \date 03-13-09
 */

#include "../include/main.h"
#ifdef IS_PARALLEL
  #include "../mandel/parallel.h"
  #include "../randmat/parallel.h"
  #include "../half/parallel.h"
  #include "../invperc/parallel.h"
  #include "../thresh/parallel.h"
  #include "../life/parallel.h"
  #include "../winnow/parallel.h"
  #include "../norm/parallel.h"
  #include "../hull/parallel.h"
  #include "../outer/parallel.h"
  #include "../gauss/parallel.h"
  #include "../sor/parallel.h"
  #include "../product/parallel.h"
  #include "../vecdiff/parallel.h"
#else
  #include "../mandel/serial.h"
  #include "../randmat/serial.h"
  #include "../half/serial.h"
  #include "../invperc/serial.h"
  #include "../thresh/serial.h"
  #include "../life/serial.h"
  #include "../winnow/serial.h"
  #include "../norm/serial.h"
  #include "../hull/serial.h"
  #include "../outer/serial.h"
  #include "../gauss/serial.h"
  #include "../sor/serial.h"
  #include "../product/serial.h"
  #include "../vecdiff/serial.h"
#endif

int main(int argc, char* argv[])
{
#ifdef IS_PARALLEL
  mpi::environment env(argc, argv);
  mpi::communicator world;

#ifdef TEST_OUTPUT
  printf ("I am process %d\n", world.rank ());
#endif
#endif

  int i;

  // there are four paths that can be run
  // 1. mandel -> ... -> invperc -> ...
  // 2. mandel -> ... -> thresh -> ...
  // 3. randmat -> ... -> invperc -> ...
  // 4. randmat -> ... -> thresh -> ...
  bool run_mandel;
  bool run_invperc;

  // mandel inputs
  int2D*  mandel_matrix;
  int     mandel_nr;
  int     mandel_nc;
  real    mandel_base_x;
  real    mandel_base_y;
  real    mandel_ext_x;
  real    mandel_ext_y;

  // randmat inputs
  int2D*  randmat_matrix;
  int     randmat_nr;
  int     randmat_nc;
  int     randmat_limit;
  int     randmat_seed;

  // half inputs
  int2D*  half_matrix;
  int     half_nr;
  int     half_nc;
  
  // invperc inputs
  int2D*  invperc_matrix;
  bool2D* invperc_mask;
  int     invperc_nr;
  int     invperc_nc;  
  real    invperc_fraction;

  // thresh inputs
  int2D*  thresh_matrix;
  bool2D* thresh_mask;
  int     thresh_nr;
  int     thresh_nc;  
  real    thresh_fraction;

  // life inputs
  bool2D* life_mask;
  int     life_nr;
  int     life_nc;
  int     life_iters;

  // winnow inputs
  int2D*  winnow_matrix;
  bool2D* winnow_mask;
  int     winnow_nr;
  int     winnow_nc;
  pt1D*   winnow_pts;
  int     winnow_n;

  // norm inputs
  pt1D*   norm_vec;
  int     norm_n;
  
  // hull inputs
  pt1D*   hull_pts;
  int     hull_n;
  pt1D*   hull_result_pts;
  int     hull_result_n;

  // outer inputs
  pt1D*   outer_pts;
  real2D* outer_result_matrix;
  real1D* outer_result_vector;
  int     outer_n;

  // gauss inputs
  real2D* gauss_matrix;
  real1D* gauss_vector;
  real1D* gauss_answer;
  int     gauss_n;

  // sor inputs
  real2D* sor_matrix;
  real1D* sor_vector;
  real1D* sor_answer;
  int     sor_n;
  real    sor_tolerance;

  // product inputs
  real2D* product_matrix1;
  real2D* product_matrix2;
  real1D* product_vector1;
  real1D* product_vector2;
  real1D* product_result1;
  real1D* product_result2;
  int     product_nr;
  int     product_nc;

  // vecdiff inputs
  real1D* vecdiff_left;
  real1D* vecdiff_right;
  int     vecdiff_n;
  real    vecdiff_norm1_diff;

  // initialize inputs
  run_mandel = false;
  run_invperc = true;
  if (run_mandel) {
    mandel_matrix = new int2D[MAXEXT];
    mandel_nr = MAXEXT;
    mandel_nc = MAXEXT;
    mandel_base_x = 0.0;
    mandel_base_y = 0.0;
    mandel_ext_x = 1.5;
    mandel_ext_y = 1.5;
    half_matrix = mandel_matrix;
    half_nr = mandel_nr;
    half_nc = mandel_nc;
  }
  else {
    randmat_matrix = new int2D[MAXEXT];
    randmat_nr = MAXEXT;
    randmat_nc = MAXEXT;
    randmat_limit = 10;
    randmat_seed = 333;
    half_matrix = randmat_matrix;
    half_nr = randmat_nr;
    half_nc = randmat_nc;
  }
  if (run_invperc) {
    invperc_matrix = half_matrix;
    invperc_mask = new bool2D[MAXEXT];
    invperc_nr = half_nr;
    invperc_nc = half_nc;
    invperc_fraction = 0.5;
    // must initialize mask with zeros for invperc
    memset (invperc_mask, 0, sizeof(boolean) * invperc_nr * invperc_nc);
    life_mask = invperc_mask;
    life_nr = invperc_nr;
    life_nc = invperc_nc;
    winnow_matrix = invperc_matrix;
  }
  else {
    thresh_matrix = half_matrix;
    thresh_mask = new bool2D[MAXEXT];
    thresh_nr = half_nr;
    thresh_nc = half_nc;
    thresh_fraction = 0.5;
    life_mask = thresh_mask;
    life_nr = thresh_nr;
    life_nc = thresh_nc;
    winnow_matrix = thresh_matrix;
  }
  life_iters = 10;
  winnow_mask = life_mask;
  winnow_nr = life_nr;
  winnow_nc = life_nc;
  winnow_n = MAXEXT;
  winnow_pts = new pt1D[winnow_n];
  norm_vec = winnow_pts;
  norm_n = winnow_n;
  hull_pts = norm_vec;
  hull_n = norm_n;
  hull_result_pts = new pt1D[hull_n];
  outer_pts = hull_result_pts;
  outer_result_matrix = new real2D[MAXEXT];
  outer_result_vector = new real1D[MAXEXT];
  // for gauss, make a copy of outer's output
  gauss_matrix = new real2D[MAXEXT];
  gauss_vector = new real1D[MAXEXT];
  gauss_answer = new real1D[MAXEXT];
  // for sor, use outer's output
  sor_matrix = outer_result_matrix;
  sor_vector = outer_result_vector;
  sor_answer = new real1D[MAXEXT];
  sor_tolerance = 10e-3;
  // for product for gauss, make a copy of outer's output
  product_matrix1 = outer_result_matrix;
  product_matrix2 = outer_result_matrix;
  product_vector1 = gauss_answer;
  product_vector2 = sor_answer;
  product_result1 = new real1D[MAXEXT];
  product_result2 = new real1D[MAXEXT];
  vecdiff_left = product_result1;
  vecdiff_right = product_result2;

  // TODO: allow overriding initial inputs from command line

#ifdef TEST_TIME
  INT64 start, end;
  start = get_ticks ();
#endif

#ifdef IS_PARALLEL
  // run chain in parallel

  // TODO: some implementations are using broadcasts of multiple rows at once;
  //       the row sizes might be smaller than MAXEXT, need to fix
  //       implementations to work for any nr, nc < MAXEXT.

  if (run_mandel) {
    mandel_mpi (world, mandel_matrix, mandel_nr, mandel_nc,
                mandel_base_x, mandel_base_y, mandel_ext_x, mandel_ext_y);
  }
  else {
    randmat_mpi (world, randmat_matrix, randmat_nr, randmat_nc,
                 randmat_limit, randmat_seed);
  }
  half_mpi (world, half_matrix, half_nr, half_nc);
  if (run_invperc) {
    invperc_mpi (world, invperc_matrix, invperc_mask, invperc_nr, invperc_nc,
                 invperc_fraction);
  }
  else {
    thresh_mpi (world, thresh_matrix, thresh_mask, thresh_nr, thresh_nc,
                thresh_fraction);
  }
  life_mpi (world, life_mask, life_nr, life_nc, life_iters);
  winnow_mpi (world, winnow_matrix, winnow_mask, winnow_nr, winnow_nc,
              winnow_pts, winnow_n);
  norm_mpi (world, norm_vec, norm_n);
  hull_mpi (world, hull_pts, hull_n, hull_result_pts, &hull_result_n);
  // use the number of points in convex hull
  outer_n = hull_result_n;
  outer_mpi (world, outer_pts, outer_result_matrix, outer_result_vector,
             outer_n);
  gauss_n = outer_n;
  sor_n = outer_n;
  product_nr = outer_n;
  product_nc = outer_n;
  // make a copy of outer result for gauss
  for (i = 0; i < gauss_n; i++) {
    // copy row by row since gauss_n may be smaller than MAXEXT
    memcpy (gauss_matrix[i], outer_result_matrix[i],
            sizeof (real) * gauss_n);
  }
  memcpy (gauss_vector, outer_result_vector,
          sizeof (real) * gauss_n);
  if (world.size () > 1) {
    // run gauss and sor at the same time
    // split the world into two communicators
    bool is_gauss = ((world.rank () % 2) == 0);
    mpi::communicator local = world.split (is_gauss? 0 : 1);
    gauss_mpi (local, gauss_matrix, gauss_vector, gauss_answer, gauss_n);
    sor_mpi (local, sor_matrix, sor_vector, sor_answer, sor_n, sor_tolerance);
    // broadcast gauss answer
    broadcast (world, gauss_answer, gauss_n, world.rank () % 2);
    // broadcast sor answer
    broadcast (world, sor_answer, sor_n, (world.rank () + 1) % 2);
    product_mpi (local, product_matrix1, product_vector1, product_result1,
                 product_nr, product_nc);
    product_mpi (local, product_matrix2, product_vector2, product_result2,
                 product_nr, product_nc);
    // broadcast product_result1
    broadcast (world, product_result1, product_nr, world.rank () % 2);
    // broadcast product_result2
    broadcast (world, product_result2, product_nr, (world.rank () + 1) % 2);
  }
  else {
    gauss_mpi (world, gauss_matrix, gauss_vector, gauss_answer, gauss_n);
    sor_mpi (world, sor_matrix, sor_vector, sor_answer, sor_n, sor_tolerance);
    product_mpi (world, product_matrix1, product_vector1, product_result1,
                 product_nr, product_nc);
    product_mpi (world, product_matrix2, product_vector2, product_result2,
                 product_nr, product_nc);
  }
  vecdiff_n = product_nr;
  vecdiff_mpi (world, vecdiff_left, vecdiff_right, vecdiff_n,
               &vecdiff_norm1_diff);
#else
  // run chain serially
  if (run_mandel) {
    mandel (mandel_matrix, mandel_nr, mandel_nc,
            mandel_base_x, mandel_base_y, mandel_ext_x, mandel_ext_y);
  }
  else {
    randmat (randmat_matrix, randmat_nr, randmat_nc,
             randmat_limit, randmat_seed);
  }
  half (half_matrix, half_nr, half_nc);
  if (run_invperc) {
    invperc (invperc_matrix, invperc_mask, invperc_nr, invperc_nc,
             invperc_fraction);
  }
  else {
    thresh (thresh_matrix, thresh_mask, thresh_nr, thresh_nc,
            thresh_fraction);
  }
  life (life_mask, life_nr, life_nc, life_iters);
  winnow (winnow_matrix, winnow_mask, winnow_nr, winnow_nc,
          winnow_pts, winnow_n);
  norm (norm_vec, norm_n);
  hull (hull_pts, hull_n, hull_result_pts, &hull_result_n);
  // use the number of points in convex hull
  outer_n = hull_result_n;
  outer (outer_pts, outer_result_matrix, outer_result_vector,
         outer_n);
  gauss_n = outer_n;
  sor_n = outer_n;
  product_nr = outer_n;
  product_nc = outer_n;
  // make a copy of outer result for gauss
  for (i = 0; i < gauss_n; i++) {
    // copy row by row since gauss_n may be smaller than MAXEXT
    memcpy (gauss_matrix[i], outer_result_matrix[i],
            sizeof (real) * gauss_n);
  }
  memcpy (gauss_vector, outer_result_vector,
          sizeof (real) * gauss_n);
  gauss (gauss_matrix, gauss_vector, gauss_answer, gauss_n);
  sor (sor_matrix, sor_vector, sor_answer, sor_n, sor_tolerance);
  product (product_matrix1, product_vector1, product_result1,
           product_nr, product_nc);
  product (product_matrix2, product_vector2, product_result2,
           product_nr, product_nc);
  vecdiff_n = product_nr;
  vecdiff (vecdiff_left, vecdiff_right, vecdiff_n,
           &vecdiff_norm1_diff);
#endif

  printf ("vecdiff_norm1_diff is %lg\n", vecdiff_norm1_diff);

#ifdef TEST_TIME
  end = get_ticks ();
  print_elapsed_time (start, end);
#endif

  // free memory
  // TODO: free memory as soon as it is not needed rather than at the end
  if (run_mandel) {
    delete [] mandel_matrix;
  }
  else {
    delete [] randmat_matrix;
  }
  if (run_invperc) {
    delete [] invperc_mask;
  }
  else {
    delete [] thresh_mask;
  }
  delete [] winnow_pts;
  delete [] hull_result_pts;
  delete [] outer_result_matrix;
  delete [] outer_result_vector;
  delete [] gauss_matrix;
  delete [] gauss_vector;
  delete [] gauss_answer;
  delete [] sor_answer;
  delete [] product_result1;
  delete [] product_result2;

  return 0;
}