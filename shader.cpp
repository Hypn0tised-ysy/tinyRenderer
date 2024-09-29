#include "shader.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <algorithm>
#include <cmath>
extern Model *model;
extern TGAImage texture;
extern Vec3f light;
extern Vec3f spot_light;
extern Vec3f camera;
Matrix model_matrix;
Matrix view;
Matrix projection;
Matrix viewport;
Matrix rotation;
// gouraud shader
Vec3f GouraudShader::vertex(int nth_face, int nth_vertex) {
  Vec3f world_coordinate = model->vert(nth_face, nth_vertex);
  uv[nth_vertex] = model->uv(nth_face, nth_vertex);
  normal[nth_vertex] = proj<3>(
      (view * model_matrix).invert_transpose() *
      embed<4>(model->norm(
          nth_face,
          nth_vertex))); // 用于变换法向量的矩阵应该是变换模型坐标的矩阵的逆的转置
  return proj<3>((viewport * projection * view * model_matrix) *
                 embed<4>(world_coordinate));
}
bool GouraudShader::fragment(Vec3f &bc, TGAColor &color) {
  float r_intensity;

  Vec2f uv_coordinate(0, 0);
  Vec3f trans_light =
      proj<3>(view * model_matrix * embed<4>(light)).normalize();

  for (int i = 0; i < 3; i++) {
    float intensity = std::max(0.0f, normal[i].normalize() * trans_light);
    r_intensity += intensity * bc[i];
    uv_coordinate.x += bc[i] * uv[i].x;
    uv_coordinate.y += bc[i] * uv[i].y;
  }
  TGAColor c(texture.get(uv_coordinate) * r_intensity);
  color = c;
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// bling phong shader
Vec3f Bling_phong_shader::vertex(int nth_face, int nth_vertex) {
  Vec3f world_coordinate = model->vert(nth_face, nth_vertex);
  view_coord[nth_vertex] =
      proj<3>(view * model_matrix * embed<4>(world_coordinate));
  uv[nth_vertex] = model->uv(nth_face, nth_vertex);
  normal[nth_vertex] = proj<3>(
      (view * model_matrix).invert_transpose() *
      embed<4>(model->norm(
          nth_face,
          nth_vertex))); // 用于变换法向量的矩阵应该是变换模型坐标的矩阵的逆的转置
  return proj<3>((viewport * projection * view * model_matrix) *
                 embed<4>(world_coordinate));
}
bool Bling_phong_shader::fragment(Vec3f &bc, TGAColor &color) {
  float r_intensity = 0.0; // 计算的最终亮度
  float specular_intensity = 0.0, diffuse_intensity = 0.0,
        ambient_intensity =
            0.0; // 暂时不考虑光强分量不同的情况，比如蓝色的漫射光
  float intensity = 1.0f; // 光的强度
  float ambient_light = 10.0f;
  Vec3f trans_spot_light =
      proj<3>((view * model_matrix) * embed<4>(spot_light)); // 变换点光源位置
  Vec3f bc_view_coord = Vec3f(0, 0, 0); // 着色点插值后的坐标
  Vec2f uv_coordinate(0, 0);
  Vec3f bc_normal = Vec3f(
      0, 0,
      0); // 插值后的法向量,因为重心坐标和法向量都是单位向量，插值后可以不归一化
  for (int i = 0; i < 3; i++) {
    bc_view_coord += bc[i] * view_coord[i];
    bc_normal += bc[i] * normal[i];
    uv_coordinate += bc[i] * uv[i];
  }
  float distance = sqrt((trans_spot_light - bc_view_coord).norm());
  float r2 = distance * distance; // r square
  Vec3f l = (trans_spot_light - bc_view_coord)
                .normalize(); // 物体（着色点）到光源连线的法向量
  Vec3f v = (proj<3>(view * model_matrix * embed<4>(camera)) - bc_view_coord)
                .normalize(); // 从着色点到camera的法向量
  Vec3f h = (v + l).normalize();
  diffuse_intensity = kd * (intensity / r2) * std::max(0.0f, bc_normal * l);
  float inter1 = bc_normal.normalize() * h.normalize();
  float inter =
      pow(std::max(0.0f, bc_normal.normalize() * h.normalize()), power);
  specular_intensity =
      ks * (intensity / r2) *
      pow(std::max(0.0f, bc_normal.normalize() * h.normalize()), power);
  ambient_intensity = ka * ambient_light;
  r_intensity = std::min(
      diffuse_intensity + specular_intensity + ambient_intensity, 1.0f);
  TGAColor c(texture.get(uv_coordinate) * r_intensity);
  color = c;
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// matrix
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
void Rotation(Vec3f &eye, Vec3f &center, Vec3f &up) {
  rotation = Matrix::identity();
  Vec3f u, v, w, gaze = center - eye;
  w = (-gaze).normalize();
  u = (up ^ w).normalize();
  v = w ^ u;
  for (int i = 0; i < 3; i++) {
    rotation[0][i] = u[i];
    rotation[1][i] = v[i];
    rotation[2][i] = w[i];
  }
}
void ModelMatrix() { model_matrix = Matrix::identity(); }
/////////////////////////////////////////////////////////////////////////////////////////////