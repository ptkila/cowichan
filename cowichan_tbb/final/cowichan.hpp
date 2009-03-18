/**
 * Datatypes and common routines for Cowichan/TBB programs.
 */
#ifndef __cowichan_hpp__
#define __cowichan_hpp__

// BASIC HEADERS ============================================================//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <climits>
#include <limits>
using std::numeric_limits;

// THREADING BUILDING BLOCKS ================================================//
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range2d.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
using namespace tbb;

typedef blocked_range<size_t> Range;
typedef blocked_range2d<size_t,size_t> Range2D;

// BASIC TYPES ==============================================================//
#ifndef REAL_TYPE
	// use IEEE single floating-point by default
	#define REAL_TYPE float
#endif
typedef REAL_TYPE	real;
typedef unsigned int uint;

typedef uint*		IntMatrix;
typedef bool*		BoolMatrix;
typedef real*		RealMatrix;

typedef uint*		IntVector;
typedef bool*		BoolVector;
typedef real*		RealVector;

typedef RealMatrix	Matrix;
typedef RealVector	Vector;

typedef std::vector<real>	RealList;
/*
 * It is worth explicitly pointing out that IntMatrix/IntVector use uint.
 */
// STATIC AND USEFUL DEFINITIONS ============================================//
// as well as values needed for the toys that are not "inputs"
#define MAXIMUM_INT		numeric_limits<int>::max()
#define MINIMUM_INT		numeric_limits<int>::min()
#define MAXIMUM_REAL	numeric_limits<real>::min()
#define MINIMUM_REAL	numeric_limits<real>::min()
#define INFINITY_REAL	numeric_limits<real>::infinity()

#define MANDEL_INFINITY	2.0
#define MANDEL_MAX_ITER	150
#define SOR_OMEGA		0.9
#define SOR_TOLERANCE	10e-6
 
// POINT TYPE ===============================================================//
class Point {
public:

	real x, y;
	Point(real x, real y): x(x), y(y) { }
	Point(): x(0.0), y(0.0) { }
	Point(const Point& other): x(other.x), y(other.y) {}
	
	static Point minimum;
	static Point maximum;
	static Point origin;

	/**
	 * Calculates euclidean distance between two points.
	 * @return the distance between p1 and p2
	 */
	static inline real distance(const Point& p1, const Point& p2) {
		return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
	}

};

Point Point::minimum = Point(MINIMUM_REAL, MINIMUM_REAL);
Point Point::maximum = Point(MAXIMUM_REAL, MAXIMUM_REAL);
Point Point::origin  = Point(0.0, 0.0);

typedef std::vector<Point>	PointList;
typedef Point*				PointVector;
 
// COWICHAN DEFINITIONS =====================================================//
// aka. "inputs" to the toys, and chaining functions.
class Cowichan {
public:
	// common
	static int NROWS;
	static int NCOLS;
	static int NELTS;
	// game of life
	static int NUMGEN;
	// mandelbrot
	static real x0, y0, dx, dy;
	// threshold
	static real PERCENT;
	// percolation
	static int NFILL;
	// seed value for simple random number generator
	static uint SEED;

public: // chaining functions

	static void mandel(IntMatrix* matrix);
	static void randmat(IntMatrix* matrix);
	static void half(IntMatrix matrixIn, IntMatrix* matrixOut);
	static void invperc(IntMatrix matrix, BoolMatrix* mask);
	static void thresh(IntMatrix matrix, BoolMatrix* mask);
	static void life(BoolMatrix matrixIn, BoolMatrix* matrixOut);
	static void winnow(IntMatrix matrix, BoolMatrix mask, PointList** points);
	static void norm(PointList* pointsIn, PointList** pointsOut);
	static void hull(PointList* pointsIn, PointList** pointsOut);
	static void outer(PointList* points, Matrix* matrix, Vector* vector);
	static void gauss(Matrix matrix, Vector target, Vector* solution);
	static void sor(Matrix matrix, Vector target, Vector* solution);
	static void product(Matrix matrix, Vector actual, Vector candidate, real* e);

public:

	/**
	 * Runs the cowichan problem set, chained together.
	 * @param numThreads	the number of threads to spawn using TBB.
	 * @param use_randmat	true: generate a random matrix.
	 * 						false: use a window of the mandelbrot set.
	 * @param use_thresh	true: use image thresholding for int->bool.
	 *						false: use invasion percolation for int->bool.
	 */
	static void run(int numThreads, bool use_randmat, bool use_thresh);
	
};

// default values for the toys.
int Cowichan::NROWS = 200;
int Cowichan::NCOLS = 200;
int Cowichan::NELTS = 200;
int Cowichan::NUMGEN = 200;
real Cowichan::x0 = -0.1;
real Cowichan::y0 = -0.1;
real Cowichan::dx = 0.2;
real Cowichan::dy = 0.2;
real Cowichan::PERCENT = 0.25;
int Cowichan::NFILL = 500;
uint Cowichan::SEED = 681304;

// UTILITY FUNCTIONS ========================================================//

#define MATRIX_RECT(mtrx,row,col)	(mtrx)[(row)*Cowichan::NCOLS + col]
#define MATRIX_SQUARE(mtrx,row,col)	(mtrx)[(row)*Cowichan::NELTS + col]
#define MATRIX						MATRIX_SQUARE
#define VECTOR(vect,row)			(vect)[row]
#define DIAG(mtrx,v)				(mtrx)[v*Cowichan::NELTS + v]

#define NEW_MATRIX_SQUARE(__type)	(new __type[Cowichan::NELTS * Cowichan::NELTS])
#define NEW_MATRIX_RECT(__type)		(new __type[Cowichan::NROWS * Cowichan::NCOLS])
#define NEW_VECTOR(__type)			(new __type[Cowichan::NELTS])

#define COWICHAN_PARALLEL	srand(time(0)); task_scheduler_init init;
#define COWICHAN_SINGLE		srand(time(0)); task_scheduler_init init(1);

#define COWICHAN(threads)	srand(time(0)); task_scheduler_init init(threads);

/**
 * Returns a pseudorandom number ~ U[mean - range, mean + range].
 */
real uniform(real mean, real range) {
	return (rand() / (real)RAND_MAX) * (2.0f * range) - range + mean;
}

// DEBUGGING FUNCTIONS ======================================================//

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
 
#endif

