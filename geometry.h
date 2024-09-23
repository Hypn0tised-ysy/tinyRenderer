#ifndef __MYGEOMETRY_H__
#define __MYGEOMETRY_H__
#include <cassert>
#include <cmath>
#include <cstddef>
#include <ostream>
#include <vector>
// 首先是二阶向量
/*
向量功能：
1.加法
2.减法
3.乘法（向量点乘）
4.乘法（向量数乘）
5.输出向量内容
*/
// TODO:1.完善const逻辑 2.补充norm函数
/*
template <typename t> struct Vec3;
template <typename t> struct Vec2 {
  union {
    struct {
      t x, y;
    };
    struct {
      t u, v;
    };
    t raw[2];
  };
  explicit Vec2() : x(0), y(0) {}
  Vec2(t _x, t _y) : x(_x), y(_y) {}
  inline void operator=(const Vec2<t> &v2) { x = v2.x, y = v2.y; }
  inline Vec2<t> operator+(const Vec2<t> &v2) {
    return Vec2<t>(x + v2.x, y + v2.y);
  }
  inline Vec2<t> operator-(const Vec2<t> &v2) {
    return Vec2<t>(x - v2.x, y - v2.y);
  }
  inline t operator*(const Vec2<t> &v2) { return x * v2.x + y * v2.y; }
  inline Vec2<t> operator*(const t factor) {
    return Vec2<t>(factor * x, factor * y);
  }
  Vec2<t> &normalize() {
    float norm = sqrt(x * x + y * y);
    if (x == 0 && y == 0) // 0 vector
      return *this;
    x = x / norm;
    y = y / norm;
    return *this;
  }
  template <typename>
  friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v2);
  template <typename> friend Vec2<t> operator*(t factor, Vec2<t> &v2);
};

template <typename t> std::ostream &operator<<(std::ostream &s, Vec2<t> &v2) {
  s << "vector's x: " << v2.x << " , " << "y: " << v2.y << "\n";
  return s;
}
template <typename t> Vec2<t> operator*(t factor, Vec2<t> &v2) {
  return v2 * factor;
}
template <typename t> struct Vec3 {
  union {
    struct {
      t x, y, z;
    };
    t raw[3];
  };
  explicit Vec3() : x(0), y(0), z(0) {}
  Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
  template <typename u> Vec3<t>(const Vec3<u> &v);
  inline Vec3<t> operator+(const Vec3<t> &v2) {
    return Vec3<t>(x + v2.x, y + v2.y, z + v2.z);
  }
  inline Vec3<t> operator-(const Vec3<t> &v2) {
    return Vec3<t>(x - v2.x, y - v2.y, z - v2.z);
  }
  inline t operator*(const Vec3<t> &v2) {
    return x * v2.x + y * v2.y + z * v2.z;
  }
  inline Vec3<t> operator*(const t factor) {
    return Vec3<t>(factor * x, factor * y, factor * z);
  }
  inline Vec3<t> operator/(const t factor) {
    return Vec3<t>(x / factor, y / factor, z / factor);
  }
  inline Vec3<t> operator^(const Vec3<t> &v2) {
    return Vec3<t>(y * v2.z - z * v2.y, -(x * v2.z - z * v2.x),
                   x * v2.y - y * v2.x);
  }
  inline Vec3<t> operator-() { return Vec3<t>(-x, -y, -z); }
  inline t operator[](const int i) {
    assert(i >= 0 && i <= 3);
    return raw[i];
  }
  Vec3<t> &normalize() {
    if (x == 0 && y == 0 && z == 0)
      return *this;
    float norm = sqrt(x * x + y * y + z * z);
    x = x / norm;
    y = y / norm;
    z = z / norm;
    return *this;
  } // 归一化
  float norm() { return sqrt(x * x + y * y + z * z); }
  template <typename>
  friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v2);
  template <typename> friend Vec3<t> operator*(t factor, Vec3<t> &v2);
};

template <typename t> std::ostream &operator<<(std::ostream &s, Vec3<t> &v2) {
  s << "vector's x: " << v2.x << " , " << "y: " << v2.y << " , "
    << "z= " << v2.z << "\n";
  return s;
}
template <typename t> Vec3<t> operator*(t factor, Vec3<t> &v2) {
  return v2 * factor;
}
// 显式特化模板：从 Vec3<float> 转换到 Vec3<int>
template <>
template <>
Vec3<int>::Vec3(const Vec3<float> &v); // float到int使用四舍五入的舍入方式

// 显式特化模板：从 Vec3<int> 转换到 Vec3<float>
template <> template <> Vec3<float>::Vec3(const Vec3<int> &v);

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

constexpr int defaultRank = 4;
class Matrix {
private:
  std::vector<std::vector<float>> m;
  int row, column;

public:
  Matrix(int _row = defaultRank, int _column = defaultRank)
      : m(std::vector<std::vector<float>>(_row,
                                          std::vector<float>(_column, 0.0f))),
        row(_row), column(_column){};
  Vec3f m2v(); // 把四维homogenous坐标转化回三维坐标
  Matrix operator*(Matrix m2);
  Matrix transpose(); // 转置
  Matrix inverse();   // 求逆
  inline std::vector<float> &operator[](const int row) { return m[row]; };
  friend std::ostream &operator<<(std::ostream &out,
                                  Matrix &m); // 输出矩阵元素
};
Matrix v2m(Vec3f &vec); // 把三维坐标转化为四维的homogenous坐标
Matrix identity(int dimension); // 返回阶数为dimension的标准矩阵
#endif
*/
// 向量模板
template <size_t dim, typename T> struct vec {
  vec() {
    for (size_t i = dim; i--; data[i] = T())
      ;
  }; // 调用T的默认构造函数，如果T是内置类型比如int，默认会是0
  T &operator[](int idx) {
    assert(0 <= idx && idx < dim);
    return data[idx];
  } // idx类型为unsigned int可能导致负数被理解成一个很大的正数
  T const &operator[](int idx) const {
    assert(0 <= idx && idx < dim);
    return data[idx];
  }

private:
  T data[dim];
};
/////////////////////////////////////////////////////////////////////////////
// 二阶向量
template <typename T> struct vec<2, T> {
  vec() {
    for (int i = 2; i--; data[i] = T())
      ;
  }
  vec(T const a, T const b) : x(a), y(b){};
  template <typename U> vec(vec<2, U> const &v);
  T &operator[](int idx) {
    assert(0 <= idx && idx < 2);
    return data[idx];
  } // idx类型为unsigned int可能导致负数被理解成一个很大的正数
  T const &operator[](int idx) const {
    assert(0 <= idx && idx < 2);
    return data[idx];
  }
  float norm() { return sqrtf(float(x * x + y * y)); };
  vec<2, T> &normalize() {
    float _norm = norm();
    for (int i = 2; i--; data[i] /= _norm)
      ;
    return *this;
  }
  union {
    struct {
      T x, y;
    };
    struct {
      T u, v;
    };
    T data[2];
  };
};
/////////////////////////////////////////////////////////////////////////////
// 三阶向量
template <typename T> struct vec<3, T> {
  vec() { x = y = z = T(); }
  template <typename U> vec(vec<3, U> const &v);
  vec(T const a, T const b, T const c) : x(a), y(b), z(c){};
  T &operator[](int idx) {
    assert(0 <= idx && idx < 3);
    return data[idx];
  } // idx类型为unsigned int可能导致负数被理解成一个很大的正数
  T const &operator[](int idx) const {
    assert(0 <= idx && idx < 3);
    return data[idx];
  }
  float norm() { return sqrtf(float(x * x + y * y + z * z)); };
  vec<3, T> &normalize() {
    float _norm = norm();
    for (int i = 3; i--; data[i] /= _norm)
      ;
    return *this;
  }

  union {
    struct {
      T x, y, z;
    };
    T data[3];
  };
};
// 四阶向量

// 向量运算
// 把向量运算都写成非成员函数，这样可以不用反复重写
// 加
template <size_t dim, typename T>
vec<dim, T>
operator+(vec<dim, T> lhs,
          vec<dim, T> const &rhs) // left hand side and right hand side
{
  for (int i = dim; i--; lhs[i] += rhs[i])
    ;
  return lhs;
}
// 减
template <size_t dim, typename T>
vec<dim, T>
operator-(vec<dim, T> lhs,
          vec<dim, T> const &rhs) // left hand side and right hand side
{
  for (int i = dim; i--; lhs[i] -= rhs[i])
    ;
  return lhs;
}
template <size_t dim, typename T> vec<dim, T> operator-(vec<dim, T> const &v) {
  vec<dim, T> result;
  for (int i = dim; i--; result[i] = -v[i])
    ;
  return result;
}
// 乘
template <size_t dim, typename T>
T operator*(vec<dim, T> const &lhs,
            vec<dim, T> const
                &rhs) // left hand side and right hand
                      // side,乘法的话int类型向量乘float同样比较麻烦，暂时不管
{
  T result = T();
  for (int i = dim; i--; result += lhs[i] * rhs[i])
    ;
  return result;
}
template <size_t dim, typename T>
vec<dim, T> operator*(vec<dim, T> lhs,
                      T const rhs) // left hand side and right hand side
{
  for (int i = dim; i--; lhs[rhs] *= rhs)
    ;
  return lhs;
}
template <size_t dim, typename T>
vec<dim, T> operator*(T const lhs,
                      vec<dim, T> rhs) // left hand side and right hand side
{
  for (int i = dim; i--; rhs[i] *= lhs)
    ;
  return rhs;
}
// 除
template <size_t dim, typename T>
vec<dim, T>
operator/(vec<dim, T> lhs,
          T const rhs) // left hand side and right hand
                       // side,int类型向量除以float的情况会麻烦一点，暂时不管
{
  for (int i = dim; i--; lhs[i] /= rhs)
    ;
  return lhs;
}
// 叉乘
template <typename T>
vec<3, T> operator^(vec<3, T> const &lhs, vec<3, T> const &rhs) {
  vec<3, T> result;
  result.x = lhs.y * rhs.z - lhs.z * rhs.y;
  result.y = -(lhs.x * rhs.z - lhs.z * rhs.x);
  result.z = lhs.x * rhs.y - lhs.y * rhs.x;
  return result;
}
// 输出
template <size_t dim, typename T>
std::ostream &operator<<(std::ostream &out, vec<dim, T> &v) {
  for (size_t i = 0; i < dim; i++) {
    out << v[i] << " ";
  }
  return out;
}
// float和int类型向量的转换
/*template <> template <> vec<3, int>::vec(vec<3, float> const &v) {
  for (int i = 3; i--; (*this)[i] = int(v[i] + 0.5))
    ;
}
template <> template <> vec<3, float>::vec(vec<3, int> const &v) {
  for (int i = 3; i--; (*this)[i] = float(v[i]))
    ;
}
*/
// 不同阶向量的转换
// 低维到高维
template <size_t len, size_t dim, typename T>
vec<len, T> embed(vec<dim, T> const &v, T fill = 1) {
  vec<len, T> result;
  for (int i = len; i--; result[i] = i < dim ? v[i] : fill)
    ;
  return result;
}
// 高维到低维
template <size_t len, size_t dim, typename T>
vec<len, T> proj(vec<dim, T> const &v) {
  vec<len, T> result;
  for (int i = len; i--; result[i] = v[i])
    ;
  return result;
}
// 矩阵
template <size_t DimCols, size_t DimRows, typename T> class mat;
template <size_t DIM, typename T> struct dt { // determinant
  static T det(const mat<DIM, DIM, T> &src) {
    T ret = 0;
    for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i))
      ;
    return ret;
  }
};

template <typename T> struct dt<1, T> {
  static T det(const mat<1, 1, T> &src) { return src[0][0]; }
};

/////////////////////////////////////////////////////////////////////////////////

template <size_t DimRows, size_t DimCols, typename T> class mat {
  vec<DimCols, T> rows[DimRows];

public:
  mat() {}

  vec<DimCols, T> &operator[](const size_t idx) {
    assert(idx < DimRows);
    return rows[idx];
  }

  const vec<DimCols, T> &operator[](const size_t idx) const {
    assert(idx < DimRows);
    return rows[idx];
  }

  vec<DimRows, T> col(const size_t idx) const {
    assert(idx < DimCols);
    vec<DimRows, T> ret;
    for (size_t i = DimRows; i--; ret[i] = rows[i][idx])
      ;
    return ret;
  }

  void set_col(size_t idx, vec<DimRows, T> v) {
    assert(idx < DimCols);
    for (size_t i = DimRows; i--; rows[i][idx] = v[i])
      ;
  }

  static mat<DimRows, DimCols, T> identity() {
    mat<DimRows, DimCols, T> ret;
    for (size_t i = DimRows; i--;)
      for (size_t j = DimCols; j--; ret[i][j] = (i == j))
        ;
    return ret;
  }

  T det() const { return dt<DimCols, T>::det(*this); } // 计算行列式

  mat<DimRows - 1, DimCols - 1, T>
  get_minor(size_t row, size_t col) const { // 获得小一阶的矩阵
    mat<DimRows - 1, DimCols - 1, T> ret;
    for (size_t i = DimRows - 1; i--;)
      for (size_t j = DimCols - 1; j--;
           ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1])
        ;
    return ret;
  }

  T cofactor(size_t row, size_t col) const { // 计算代数余子式
    return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
  }

  mat<DimRows, DimCols, T> adjugate() const {
    mat<DimRows, DimCols, T> ret;
    for (size_t i = DimRows; i--;)
      for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j))
        ;
    return ret;
  }

  mat<DimRows, DimCols, T> invert_transpose() {
    mat<DimRows, DimCols, T> ret = adjugate();
    T tmp = ret[0] * rows[0];
    return ret / tmp;
  }
};

/////////////////////////////////////////////////////////////////////////////////

template <size_t DimRows, size_t DimCols, typename T>
vec<DimRows, T> operator*(const mat<DimRows, DimCols, T> &lhs,
                          const vec<DimCols, T> &rhs) {
  vec<DimRows, T> ret;
  for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs)
    ;
  return ret;
}

template <size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator*(const mat<R1, C1, T> &lhs, const mat<C1, C2, T> &rhs) {
  mat<R1, C2, T> result;
  for (size_t i = R1; i--;)
    for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j))
      ;
  return result;
}

template <size_t DimRows, size_t DimCols, typename T>
mat<DimCols, DimRows, T> operator/(mat<DimRows, DimCols, T> lhs, const T &rhs) {
  for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs)
    ;
  return lhs;
}

template <size_t DimRows, size_t DimCols, class T>
std::ostream &operator<<(std::ostream &out, mat<DimRows, DimCols, T> &m) {
  for (size_t i = 0; i < DimRows; i++)
    out << m[i] << std::endl;
  return out;
}

/////////////////////////////////////////////////////////////////////////////////

typedef vec<2, float> Vec2f;
typedef vec<2, int> Vec2i;
typedef vec<3, float> Vec3f;
typedef vec<3, int> Vec3i;
typedef vec<4, float> Vec4f;
typedef mat<4, 4, float> Matrix;
#endif