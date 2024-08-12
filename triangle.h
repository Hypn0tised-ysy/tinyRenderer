#include "geometry.h"
#include "tgaimage.h"
class Triangle {
private:
  Vec2i a, b, c;

public:
  Triangle() : a(), b(), c() {}
  Triangle(Vec2i &_a, Vec2i &_b, Vec2i &_c) : a(_a), b(_b), c(_c) {}
};
