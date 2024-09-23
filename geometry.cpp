#include "geometry.h"
#include <cassert>
/*
// 显式特化模板：从 Vec3<float> 转换到 Vec3<int>
template <>
template <>
Vec3<int>::Vec3(const Vec3<float> &v)
    : x(static_cast<int>(v.x + 0.5)), y(static_cast<int>(v.y + 0.5)),
      z(static_cast<int>(v.z + 0.5)) {}

// 显式特化模板：从 Vec3<int> 转换到 Vec3<float>
template <>
template <>
Vec3<float>::Vec3(const Vec3<int> &v)
    : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)),
      z(static_cast<float>(v.z)) {}
*/
/*
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
Matrix Matrix::transpose() {
  Matrix result(column, row);
  for (int r = 0; r < row; ++r) {
    for (int c = 0; c < column; ++c) {
      result[c][r] = m[r][c];
    }
  }
  return result;
}
std::ostream &operator<<(std::ostream &out, Matrix &m) {
  for (int r = 0; r < m.row; r++) {
    for (int j = 0; j < m.column; j++) {
      out << m[r][j] << "\t";
    }
    out << "\n";
  }
  out << "\n";
  return out;
}
*/
// 类型转换
template <>
template <>
vec<3, int>::vec(const vec<3, float> &v)
    : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}
template <>
template <>
vec<3, float>::vec(const vec<3, int> &v) : x(v.x), y(v.y), z(v.z) {}
template <>
template <>
vec<2, int>::vec(const vec<2, float> &v)
    : x(int(v.x + .5f)), y(int(v.y + .5f)) {}
template <>
template <>
vec<2, float>::vec(const vec<2, int> &v) : x(v.x), y(v.y) {}