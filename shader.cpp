#include "shader.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <algorithm>
extern Model *model;
extern TGAImage texture;
extern Vec3f light;
Vec3f Shader::vertex(int nth_face, int nth_vertex) {
  Vec3f world_coordinate = model->vert(nth_face, nth_vertex);
  uv[nth_vertex] = model->uv(nth_face, nth_vertex);
  normal[nth_vertex] = model->norm(nth_face, nth_vertex);
  return (trans * v2m(world_coordinate)).m2v();
}
bool Shader::fragment(Vec3f &bc, TGAColor &color) {
  float r_intensity = 0.0;
  Vec2f uv_coordinate(0, 0);
  for (int i = 0; i < 3; i++) {
    float intensity = std::max(0.0f, normal[i].normalize() * light.normalize());
    r_intensity += intensity * bc[i];
    uv_coordinate.x += bc[i] * uv[i].x;
    uv_coordinate.y += bc[i] * uv[i].y;
  }
  TGAColor c(texture.get(uv_coordinate) * r_intensity);
  color = c;
  return false;
}