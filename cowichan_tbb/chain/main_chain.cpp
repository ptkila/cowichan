#include "cowichan.hpp"

// default values for the toys.
int Cowichan::NROWS = 10000;
int Cowichan::NCOLS = 10000;
int Cowichan::NELTS = 10000;
int Cowichan::NUMGEN = 200;
real Cowichan::x0 = -2.0;
real Cowichan::y0 = -1.0;
real Cowichan::dx = 3.0;
real Cowichan::dy = 2.0;
real Cowichan::PERCENT = 0.75;
int Cowichan::NFILL = 200;
uint Cowichan::SEED = 681304;

/**
 * Runs all problems in the cowichan problem set, chained together.
 * @param numThreads	the number of threads to spawn using TBB.
 * @param use_randmat	true: generate a random matrix.
 * 						false: use a window of the mandelbrot set.
 * @param use_thresh	true: use image thresholding for int->bool.
 *						false: use invasion percolation for int->bool.
 */
void Cowichan::run(int numThreads, bool use_randmat, bool use_thresh) {
	IntMatrix matrix;
	IntMatrix newMatrix;
	Matrix realmx;
	Vector vector, x_sor, x_gauss;
	BoolMatrix bm;
	PointList* points;
	real e_gauss, e_sor;

	INT64 start, end;
	end = get_ticks();

	// set up for the number of threads we will use
	COWICHAN(numThreads);		
	timeInfo(&start, &end, "init");
		
	// Cowichan step #1
	if (use_randmat) {
		randmat(&matrix);
		timeInfo(&start, &end, "random matrix");
	} else {
		mandel(&matrix);
		timeInfo(&start, &end, "mandelbrot set");
	}
	
	// Cowichan step #2		
	half(matrix, &newMatrix);
	timeInfo(&start, &end, "halving shuffle");

	// Cowichan step #3
	if (use_thresh) {
		thresh(newMatrix, &bm);
		timeInfo(&start, &end, "image thresholding");
	} else {
		invperc(newMatrix, &bm);
		timeInfo(&start, &end, "invasion percolation");
	}

	// Cowichan step #4
	life(bm, &bm);
	timeInfo(&start, &end, "game of life");
	
	// Cowichan step #5
	winnow(newMatrix, bm, &points);
	timeInfo(&start, &end, "weighted point selection");

	// Cowichan step #6
	hull(points, &points);
	timeInfo(&start, &end, "convex hull");

	// Cowichan step #7
	norm(points, &points);
	timeInfo(&start, &end, "point normalization");
//	print(*points);

	// Cowichan step #8
	outer(points, &realmx, &vector);
	timeInfo(&start, &end, "outer product");

	// Cowichan step #9
	gauss(realmx, vector, &x_gauss);
	timeInfo(&start, &end, "gaussian elimination");
	sor(realmx, vector, &x_sor);
	timeInfo(&start, &end, "successive over-relaxation");
	
	// Cowichan step #10
	product(realmx, vector, x_gauss, &e_gauss);
	product(realmx, vector, x_sor, &e_sor);
	timeInfo(&start, &end, "two matrix-vector product");	
	
}

Point Point::minimum = Point(MINIMUM_REAL, MINIMUM_REAL);
Point Point::maximum = Point(MAXIMUM_REAL, MAXIMUM_REAL);
Point Point::origin  = Point(0.0, 0.0);	
	
/*****************************************************************************/

/**
 * Returns a pseudorandom number ~ U[mean - range, mean + range].
 */
real uniform(real mean, real range) {
	return (rand() / (real)RAND_MAX) * (2.0f * range) - range + mean;
}

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
void printAxb(Matrix matrix, Vector answer, Vector vector) {
	std::cout.precision(5);
	for (int row = 0; row < Cowichan::NELTS; ++row) {

		// print out the matrix
		std::cout << " [ ";
		for (int col = 0; col < Cowichan::NELTS; ++col) {
			std::cout << MATRIX(matrix, row,col) << "\t";
		}
		
		// print out the answer
		std::cout << "] [ " << VECTOR(answer, row) << " ]\t";

		// print out the vector
		if (row == int(Cowichan::NELTS / 2)) {
			std::cout << "= [ ";
		} else {
			std::cout << "  [ ";
		}
		std::cout << VECTOR(vector, row) << " ]" << std::endl;
		
	}
}

/*****************************************************************************/

// defines which test to run, if not defined then chain is run
#define TEST13

/**
 * The entry point of the Cowichan/TBB problem set.
 */
int main(int argc, char** argv) {

#if !defined(TEST1) && !defined(TEST2) && !defined(TEST3) && !defined(TEST4) \
  && !defined(TEST5) && !defined(TEST6) && !defined(TEST7) && !defined(TEST8) \
  && !defined(TEST9) && !defined(TEST10) && !defined(TEST11) \
  && !defined(TEST12) && !defined(TEST13)
  // run chain
  Cowichan::run(2, true, false);
#else
  // run individual tests
  INT64 start, end;
  end = get_ticks();

	// set up for the number of threads we will use
  COWICHAN(2);
  timeInfo(&start, &end, "init");

  // 1. testing randmat
#ifdef TEST1
  IntMatrix matrix;
  Cowichan::randmat(&matrix);
  timeInfo(&start, &end, "random matrix");
#endif

  // 2. testing mandel
#ifdef TEST2
  IntMatrix matrix;
  Cowichan::mandel(&matrix);
  timeInfo(&start, &end, "mandelbrot set");
#endif

  // 3. testing half
#ifdef TEST3
  IntMatrix matrix;
  IntMatrix newMatrix;
  matrix = NEW_MATRIX_RECT(uint);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NROWS; r++) {
    for (c = 0; c < Cowichan::NCOLS; c++) {
      MATRIX_RECT(matrix, r, c) = rand () % limit;
    }
  }
	timeInfo(&start, &end, "init halving shuffle");

  Cowichan::half(matrix, &newMatrix);
  timeInfo(&start, &end, "halving shuffle");
#endif

  // 4. testing thresh
#ifdef TEST4
  IntMatrix matrix;
	BoolMatrix bm;
  matrix = NEW_MATRIX_RECT(uint);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NROWS; r++) {
    for (c = 0; c < Cowichan::NCOLS; c++) {
      MATRIX_RECT(matrix, r, c) = rand () % limit;
    }
  }
  timeInfo(&start, &end, "init image thresholding");

  Cowichan::thresh(matrix, &bm);
  timeInfo(&start, &end, "image thresholding");
#endif

  // 5. testing invperc
#ifdef TEST5
  IntMatrix matrix;
	BoolMatrix bm;
  matrix = NEW_MATRIX_RECT(uint);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NROWS; r++) {
    for (c = 0; c < Cowichan::NCOLS; c++) {
      MATRIX_RECT(matrix, r, c) = rand () % limit;
    }
  }
  timeInfo(&start, &end, "init invasion percolation");

  Cowichan::invperc(matrix, &bm);
  timeInfo(&start, &end, "invasion percolation");
#endif

  // 6. testing life
#ifdef TEST6
	BoolMatrix bm;
  bm = NEW_MATRIX_RECT(bool);
  int r, c;
  for (r = 0; r < Cowichan::NROWS; r++) {
    for (c = 0; c < Cowichan::NCOLS; c++) {
      MATRIX_RECT(bm, r, c) = ((rand () % 2) == 0);
    }
  }
  timeInfo(&start, &end, "init game of life");

  Cowichan::life(bm, &bm);
	timeInfo(&start, &end, "game of life");
#endif

  // 7. testing winnow
#ifdef TEST7
  IntMatrix matrix;
	BoolMatrix bm;
	PointList* points;
  matrix = NEW_MATRIX_RECT(uint);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NROWS; r++) {
    for (c = 0; c < Cowichan::NCOLS; c++) {
      MATRIX_RECT(matrix, r, c) = rand () % limit;
    }
  }
  bm = NEW_MATRIX_RECT(bool);
  for (r = 0; r < Cowichan::NROWS; r++) {
    for (c = 0; c < Cowichan::NCOLS; c++) {
      MATRIX_RECT(bm, r, c) = ((rand () % 2) == 0);
    }
  }
  timeInfo(&start, &end, "init weighted point selection");

  Cowichan::winnow(matrix, bm, &points);
  timeInfo(&start, &end, "weighted point selection");
#endif

  // 8. testing hull
#ifdef TEST8
	PointList* points;
  points = new PointList();
  int i;
  int limit = 10;
  for (i = 0; i < Cowichan::NELTS; i++) {
    Point *p = new Point((real) (rand () % limit), (real) (rand () % limit));
    points->push_back (*p);
    // no cleanup...
  }
  timeInfo(&start, &end, "init convex hull");

  Cowichan::hull(points, &points);
  timeInfo(&start, &end, "convex hull");
#endif

  // 9. testing norm
#ifdef TEST9
	PointList* points;
  points = new PointList();
  int i;
  int limit = 10;
  for (i = 0; i < Cowichan::NELTS; i++) {
    Point *p = new Point((real) (rand () % limit), (real) (rand () % limit));
    points->push_back (*p);
    // no cleanup...
  }
  timeInfo(&start, &end, "init point normalization");

  Cowichan::norm(points, &points);
	timeInfo(&start, &end, "point normalization");
#endif

  // 10. testing outer
#ifdef TEST10
	PointList* points;
	Matrix realmx;
	Vector vector;
  points = new PointList();
  int i;
  int limit = 10;
  for (i = 0; i < Cowichan::NELTS; i++) {
    Point *p = new Point((real) (rand () % limit), (real) (rand () % limit));
    points->push_back (*p);
    // no cleanup...
  }
  timeInfo(&start, &end, "init outer product");

  Cowichan::outer(points, &realmx, &vector);
  timeInfo(&start, &end, "outer product");
#endif

  // 11. testing gauss
#ifdef TEST11
	Matrix realmx;
	Vector vector, x_gauss;
  realmx = NEW_MATRIX_SQUARE(real);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NELTS; r++) {
    for (c = 0; c < Cowichan::NELTS; c++) {
      MATRIX(realmx, r, c) = rand () % limit;
    }
  }
  vector = NEW_VECTOR(real);
  for (r = 0; r < Cowichan::NELTS; r++) {
    VECTOR(vector, r) = rand () % limit;
  }
  timeInfo(&start, &end, "init gaussian elimination");

  Cowichan::gauss(realmx, vector, &x_gauss);
  timeInfo(&start, &end, "gaussian elimination");
#endif

  // 12. testing sor
#ifdef TEST12
	Matrix realmx;
	Vector vector, x_sor;
  realmx = NEW_MATRIX_SQUARE(real);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NELTS; r++) {
    for (c = 0; c < Cowichan::NELTS; c++) {
      MATRIX(realmx, r, c) = rand () % limit;
    }
  }
  vector = NEW_VECTOR(real);
  for (r = 0; r < Cowichan::NELTS; r++) {
    VECTOR(vector, r) = rand () % limit;
  }
  timeInfo(&start, &end, "init successive over-relaxation");

  Cowichan::sor(realmx, vector, &x_sor);
	timeInfo(&start, &end, "successive over-relaxation");
#endif

  // 13. testing product and vecdiff
#ifdef TEST13
	Matrix realmx;
	Vector vector, x;
  real e;
  realmx = NEW_MATRIX_SQUARE(real);
  int r, c;
  int limit = 10;
  for (r = 0; r < Cowichan::NELTS; r++) {
    for (c = 0; c < Cowichan::NELTS; c++) {
      MATRIX(realmx, r, c) = rand () % limit;
    }
  }
  vector = NEW_VECTOR(real);
  x = NEW_VECTOR(real);
  for (r = 0; r < Cowichan::NELTS; r++) {
    VECTOR(vector, r) = rand () % limit;
    VECTOR(x, r) = rand () % limit;
  }
  timeInfo(&start, &end, "init matrix-vector product and vecdiff");

  Cowichan::product(realmx, vector, x, &e);
	timeInfo(&start, &end, "matrix-vector product and vecdiff");
#endif

#endif // end of individual tests

	return 0;
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
