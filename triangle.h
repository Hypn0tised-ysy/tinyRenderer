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