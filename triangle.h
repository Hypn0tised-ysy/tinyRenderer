#ifndef __TRIANGLE__H__
#define __TRIANGLE__H__

#include "geometry.h"
#include "tgaimage.h"
class Triangle2D {
private:
  Vec2i a, b, c;

public:
  Triangle2D() : a(), b(), c() {}
  Triangle2D(Vec2i &_a, Vec2i &_b, Vec2i &_c) : a(_a), b(_b), c(_c) {}
};
class Triangle3D {
private:
  Vec3f a, b, c;

public:
  Triangle3D() : a(), b(), c() {}
  Triangle3D(Vec3f &_a, Vec3f &_b, Vec3f &_c) : a(_a) {}
};
class Triangle {
public:
  Vec3f world_coordinate[3]; // 三个顶点的世界坐标，在程序是NDC坐标
  Vec3i screen_coordinate[3];  // 屏幕坐标
  Vec2f texture_coordinate[3]; // 顶点对应的纹理坐标
  Vec3f normal_coordinate[3];  // 顶点对应的法向量坐标
  Triangle(){};
  inline void setWorld(Vec3f *world) {
    for (int i = 0; i < 3; ++i)
      world_coordinate[i] = world[i];
  }
  inline void setScreen(Vec3i *screen) {
    for (int i = 0; i < 3; ++i)
      screen_coordinate[i] = screen[i];
  }
  inline void setTexture(Vec2f *texture) {
    for (int i = 0; i < 3; ++i)
      texture_coordinate[i] = texture[i];
  }

  inline void setNormal(Vec3f *normal) {
    for (int i = 0; i < 3; ++i)
      normal_coordinate[i] = normal[i];
  }
  Vec3f BarycentricInterpolation(
      Vec2f &
          point); // 基于屏幕坐标求三角形的重心坐标，其实重心坐标插值实际上应该按世界坐标来计算，不过在这个项目里没有考虑那么多
};
#endif