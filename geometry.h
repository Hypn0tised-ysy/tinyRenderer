#ifndef __MYGEOMETRY_H__
#define __MYGEOMETRY_H__
#include <cmath>
#include <ostream>
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
  inline Vec2<t> operator+(const Vec2<t> &v2) {
    return Vec2<t>(x + v2.x, y + v2.y);
  }
  inline Vec2<t> operator-(const Vec2<t> &v2) {
    return Vec2<t>(x - v2.x, y - v2.y);
  }
  inline t operator*(const Vec2<t> &v2) { return x * v2.x + y * v2.y; }
  inline Vec2<t> operator*(const int factor) {
    return Vec2(factor * x, factor * y);
  }
  Vec2<t> &normalize() {
    float norm = x * x + y * y;
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
// 三维向量
/*
1.向量加法
2.向量减法
3.向量乘法（数乘和点乘）
4.输出向量内容
*/
template <typename t> struct Vec3 {
  union {
    struct {
      t x, y, z;
    };
    t raw[3];
  };
  explicit Vec3() : x(0), y(0), z(0) {}
  Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
  inline Vec3<t> operator+(const Vec3<t> &v2) {
    return Vec3<t>(x + v2.x, y + v2.y, z + v2.z);
  }
  inline Vec3<t> operator-(const Vec3<t> &v2) {
    return Vec3<t>(x - v2.x, y - v2.y, z - v2.z);
  }
  inline t operator*(const Vec3<t> &v2) {
    return x * v2.x + y * v2.y + z * v2.z;
  }
  inline Vec3<t> operator*(const int factor) {
    return Vec3(factor * x, factor * y, factor * z);
  }
  inline Vec3<t> operator^(const Vec3<t> &v2) {
    return Vec3<t>(y * v2.z - z * v2.y, -(x * v2.z - z * v2.x),
                   x * v2.y - y * v2.x);
  }
  Vec3<t> &normalize() {
    float norm = sqrt(x * x + y * y + z * z);
    if (x == 0 && y == 0 && z == 0)
      return *this;
    x = x / norm;
    y = y / norm;
    z = z / norm;
    return *this;
  } // 归一化
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
typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
#endif