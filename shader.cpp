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
  normal[nth_vertex] = proj<3, 4, float>(
      trans.invert_transpose() *
      embed<4, 3, float>(model->norm(
          nth_face,
          nth_vertex))); // 用于变换法向量的矩阵应该是变换模型坐标的矩阵的逆的转置
  return proj<3, 4, float>((trans * embed<4, 3, float>(world_coordinate)));
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