#include "shader.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <algorithm>
extern Model *model;
extern TGAImage texture;
extern Vec3f light;
Matrix model_matrix;
Matrix view;
Matrix projection;
Matrix viewport;
Vec3f GouraudShader::vertex(int nth_face, int nth_vertex) {
  Vec3f world_coordinate = model->vert(nth_face, nth_vertex);
  uv[nth_vertex] = model->uv(nth_face, nth_vertex);
  normal[nth_vertex] = proj<3>(
      (viewport * projection * view * model_matrix).invert_transpose() *
      embed<4>(model->norm(
          nth_face,
          nth_vertex))); // 用于变换法向量的矩阵应该是变换模型坐标的矩阵的逆的转置
  return proj<3>((viewport * projection * view * model_matrix) *
                 embed<4>(world_coordinate));
}
bool GouraudShader::fragment(Vec3f &bc, TGAColor &color) {
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
void Viewport(int x, int y, int width,
              int height) { //(x,y)是坐标原点，width和height分别是
  viewport = Matrix::identity();
  viewport[0][0] = width / 2.0;
  viewport[1][1] = height / 2.0;
  viewport[2][2] = 255 / 2.0;
  viewport[0][3] = x + width / 2.0;
  viewport[1][3] = y + height / 2.0;
  viewport[2][3] = 255 / 2.0;
}
void Projection(
    Vec3f eye,
    Vec3f
        center) { // 投影矩阵在homogenous坐标下可以与待投影的点无关，需要一点小小的数学技巧
  projection = Matrix::identity();
  projection[3][2] = -1.0 / (center - eye).norm();
}
void View(Vec3f &eye, Vec3f &center, Vec3f &up) {
  Matrix translation = Matrix::identity(), rotation = Matrix::identity();
  for (int i = 0; i < 3; i++) {
    translation[i][3] = -center[i];
  }
  Vec3f u, v, w, gaze = center - eye;
  w = (-gaze).normalize();
  u = (up ^ w).normalize();
  v = w ^ u;
  for (int i = 0; i < 3; i++) {
    rotation[0][i] = u[i];
    rotation[1][i] = v[i];
    rotation[2][i] = w[i];
  }
  view = rotation * translation;
}
void ModelMatrix() { model_matrix = Matrix::identity(); }