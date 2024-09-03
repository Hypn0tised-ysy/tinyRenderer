#include "geometry.h"
#include <cassert>
std::vector<float> &Matrix::operator[](const int r) {
  assert(r >= 0 && r < row);
  return m[r];
}

Matrix identity(int dimension) {
  Matrix matrix(dimension, dimension);
  for (int row = 0; row < dimension; row++) {
    for (int column = 0; column < dimension; column++) {
      row == column ? matrix[row][column] = 1.0 : matrix[row][column] = 0.0;
    }
  }
  return matrix;
}
Matrix v2m(Vec3f &vec) {
  Matrix m(4, 1);
  m[0][0] = vec.x;
  m[1][0] = vec.y;
  m[2][0] = vec.z;
  m[3][0] = 1.0;
  return m;
}
Vec3f Matrix::m2v() {
  Vec3f vec(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
  return vec;
}
Matrix Matrix::operator*(Matrix m2) {
  assert(column == m2.row);
  Matrix result(row, m2.column);
  for (int r = 0; r < row; ++r) {
    for (int c = 0; c < m2.column; ++c) {
      for (int col = 0; col < column; ++col) {
        result[r][c] += m[r][col] * m2[col][c];
      }
    }
  }
  return result;
}
std::ostream &operator<<(std::ostream &out, const Matrix &m) { return out; }
