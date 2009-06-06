#include "cowichan.hpp"

/**
 * Returns a pseudorandom number ~ U[mean - range, mean + range].
 */
real uniform(real mean, real range) {
  return (rand() / (real)RAND_MAX) * (2.0f * range) - range + mean;
}

/*****************************************************************************/

/**
 * "pretty-print" a list of points.
 */
#define PRINT_BREAK 4
void print(PointList& points) {
  int b = 0;
  for (PointList::const_iterator it = points.begin(); it != points.end(); ++it) {
    if (b == 0) std::cout << std::endl << "\t";
    std::cout << "(" << it->x << "," << it->y << ")\t";
    b = (b + 1) % PRINT_BREAK;
  }
}

/**
 * show a matrix result
 */
void Cowichan::printAxb(Matrix matrix, Vector answer, Vector vector) {
  std::cout.precision(5);
  for (int row = 0; row < n; ++row) {

    // print out the matrix
    std::cout << " [ ";
    for (int col = 0; col < n; ++col) {
      std::cout << MATRIX(matrix, row, col) << "\t";
    }
    
    // print out the answer
    std::cout << "] [ " << VECTOR(answer, row) << " ]\t";

    // print out the vector
    if (row == int(n / 2)) {
      std::cout << "= [ ";
    } else {
      std::cout << "  [ ";
    }
    std::cout << VECTOR(vector, row) << " ]" << std::endl;
    
  }
}

/*****************************************************************************/

INT64 get_ticks ()
{
  INT64 count;
#if defined(WIN32)   // Windows
  if (! QueryPerformanceCounter((LARGE_INTEGER *) &count)) {
    count = GetTickCount (); // ms
  }
#else                // Linux
  tms tm;
  count = times (&tm);
#endif               // end of WIN32/Linux definitions
  return count;
}

INT64 get_freq ()
{
  INT64 freq;
#if defined(WIN32)   // Windows
  if (! QueryPerformanceFrequency((LARGE_INTEGER *) &freq)) {
    freq = 1000; // ms
  }
#else                // Linux
  freq = sysconf (_SC_CLK_TCK);
#endif               // end of WIN32/Linux definitions
  return freq;
}

/**
 * Does a sort of swap-out, printing progress.
 */
void timeInfo(INT64 *start, INT64 *end, std::string message) {
  *start = *end;
  *end = get_ticks();
  #ifdef TEST_TIME
    std::cout << message << ": ";
    print_elapsed_time(*start, *end);
    std::cout << std::endl;
  #endif
}

void print_elapsed_time (INT64 start, INT64 end)
{
  INT64 freq = get_freq ();
  std::cout.precision(5);
  std::cout << (((double) (end - start)) / ((double) freq)) << " seconds";
  std::cout.flush();
}

/*****************************************************************************/

const char* Cowichan::CHAIN = "chain";
const char* Cowichan::MANDEL = "mandel";
const char* Cowichan::RANDMAT = "randmat";
const char* Cowichan::HALF = "half";
const char* Cowichan::INVPERC = "invperc";
const char* Cowichan::THRESH = "thresh";
const char* Cowichan::LIFE = "life";
const char* Cowichan::WINNOW = "winnow";
const char* Cowichan::NORM = "norm";
const char* Cowichan::HULL = "hull";
const char* Cowichan::OUTER = "outer";
const char* Cowichan::GAUSS = "gauss";
const char* Cowichan::SOR = "sor";
const char* Cowichan::PRODUCT = "product";
const char* Cowichan::VECDIFF = "vecdiff";

void Cowichan::main (int argc, char* argv[], bool use_randmat, bool use_thresh)
{
  if ((argc == 1) || (strcmp (argv[1], CHAIN) == 0)) {
    chain (use_randmat, use_thresh);
  }
  else {
    INT64 start, end;

    if (strcmp (argv[1], MANDEL) == 0) {
      // set up
      nr = MANDEL_NR;
      nc = MANDEL_NC;
      mandelX0 = MANDEL_X0;
      mandelY0 = MANDEL_Y0;
      mandelDx = MANDEL_DX;
      mandelDy = MANDEL_DY;

      // initialize
      IntMatrix matrix = NEW_MATRIX_RECT(uint);

      // execute
      end = get_ticks ();
      mandel (matrix);
      timeInfo(&start, &end, MANDEL);

      // clean up
      delete [] matrix;
    }
    else if (strcmp (argv[1], RANDMAT) == 0) {
      // set up
      nr = RANDMAT_NR;
      nc = RANDMAT_NC;
      seed = RANDMAT_SEED;

      // initialize
      IntMatrix matrix = NEW_MATRIX_RECT(uint);

      // execute
      end = get_ticks ();
      randmat (matrix);
      timeInfo(&start, &end, RANDMAT);

      // clean up
      delete [] matrix;
    }
    else if (strcmp (argv[1], HALF) == 0) {
      // set up
      nr = HALF_NR;
      nc = HALF_NC;
      srand(RANDMAT_SEED);

      // initialize
      IntMatrix matrixIn = NEW_MATRIX_RECT(uint);
      IntMatrix matrixOut = NEW_MATRIX_RECT(uint);
      int r, c;

      for (r = 0; r < nc; r++) {
        for (c = 0; c < nc; c++) {
          MATRIX_RECT(matrixIn, r, c) = rand () % RANDMAT_M;
        }
      }

      // execute
      end = get_ticks ();
      half (matrixIn, matrixOut);
      timeInfo(&start, &end, HALF);

      // clean up
      delete [] matrixIn;
      delete [] matrixOut;
    }
    else if (strcmp (argv[1], INVPERC) == 0) {
      invperc (NULL, NULL);
    }
    else if (strcmp (argv[1], THRESH) == 0) {
      thresh (NULL, NULL);
    }
    else if (strcmp (argv[1], LIFE) == 0) {
      life (NULL, NULL);
    }
    else if (strcmp (argv[1], WINNOW) == 0) {
      winnow (NULL, NULL, NULL);
    }
    else if (strcmp (argv[1], NORM) == 0) {
      norm (NULL, NULL);
    }
    else if (strcmp (argv[1], HULL) == 0) {
      hull (NULL, NULL);
    }
    else if (strcmp (argv[1], OUTER) == 0) {
      outer (NULL, NULL, NULL);
    }
    else if (strcmp (argv[1], GAUSS) == 0) {
      gauss (NULL, NULL, NULL);
    }
    else if (strcmp (argv[1], SOR) == 0) {
      sor (NULL, NULL, NULL);
    }
    else if (strcmp (argv[1], PRODUCT) == 0) {
      product (NULL, NULL, NULL);
    }
    else if (strcmp (argv[1], VECDIFF) == 0) {
      vecdiff (NULL, NULL, NULL);
    }
  }
}

