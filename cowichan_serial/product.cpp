#include "cowichan_serial.hpp"

void CowichanSerial::product (Matrix matrix, Vector candidate, Vector solution)
{
  int r, c;

  for (r = 0; r < n; r++) {
    solution[r] = MATRIX_SQUARE(matrix, r, 0) * candidate[0];
    for (c = 1; c < n; c++) {
      solution[r] += MATRIX_SQUARE(matrix, r, c) * candidate[c];
    }
  }
}
