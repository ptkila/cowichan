#include "cowichan_openmp.hpp"
#include <queue>

class PercPoint {
public:

  Point point;
  IntMatrix matrix;
  INT64 nc;

public:

  PercPoint(Point point): point(point) { }

  // we want to extract lowest values.
  bool operator<(const PercPoint &other) const {
    return value() > other.value();
  }

  INT_TYPE value() const {
    return MATRIX_RECT(matrix, (INT64)point.y, (INT64)point.x);
  }

};

/*****************************************************************************/

void CowichanOpenMP::invperc(IntMatrix matrix, BoolMatrix mask) {
  
  if (nr * nc < invpercNFill) {
    not_enough_points();
  }

  PercPoint pp(Point(0, 0));
  pp.matrix = matrix;
  pp.nc = nc;
  
  // "seed" with the middle value; start a priority queue.
  std::vector<PercPoint> points;

  PercPoint initialPoint(Point((real) (nr / 2), (real) (nc / 2)));
  initialPoint.matrix = matrix;
  initialPoint.nc = nc;
  points.push_back(initialPoint);
  std::make_heap(points.begin(), points.end());

  // perform invasion percolation nfill times.
  INT64 r, c;
  for (INT64 it = 0; it < invpercNFill; ++it) {

    // get the highest-priority point that hasn't already
    // been filled.
    do {
      std::pop_heap(points.begin(), points.end());
      pp = points.back();
      points.pop_back();
      r = (INT64)pp.point.y;
      c = (INT64)pp.point.x;
    } while (MATRIX_RECT(mask, r, c)); // find a free one

    // fill it.
    MATRIX_RECT(mask, r, c) = true;

    // add all of its neighbours to the party...
    
    // top neighbour
    if (r > 0) {
      PercPoint topPoint(Point((real)c, (real)r - 1));
      topPoint.matrix = matrix;
      topPoint.nc = nc;
      points.push_back(topPoint);
      push_heap(points.begin(), points.end());
    }
    
    // bottom neighbour
    if (r < (nr - 1)) {
      PercPoint bottomPoint(Point((real)c, (real)r + 1));
      bottomPoint.matrix = matrix;
      bottomPoint.nc = nc;
      points.push_back(bottomPoint);
      push_heap(points.begin(), points.end());
    }
    
    // left neighbour
    if (c > 0) {
      PercPoint leftPoint(Point((real)c - 1, (real)r));
      leftPoint.matrix = matrix;
      leftPoint.nc = nc;
      points.push_back(leftPoint);
      push_heap(points.begin(), points.end());
    }
    
    // right neighbour
    if (c < (nc - 1)) {
      PercPoint rightPoint(Point((real)c + 1, (real)r));
      rightPoint.matrix = matrix;
      rightPoint.nc = nc;
      points.push_back(rightPoint);
      push_heap(points.begin(), points.end());
    }
    
  }
  
}
