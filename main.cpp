#include "geometry.h"
#include "model.h"
#include "shader.h"
#include "tgaimage.h"
#include "triangle.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
/*
about the obj file
the vertex coordinate are based on a cubic of [-1,1]^3
we need to convert the coordinate onto 2D screen, namely the width*weight
rectangle
*/
Model *model = NULL;
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255; // 把z坐标投影到[0,255]，输出z-buffer图像以供调试
TGAImage image(width, height, TGAImage::RGB);
// texture
TGAImage texture;
float *zBuffer = new float[width * height];
Vec3f camera(1, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f light = Vec3f(1, -1, 1).normalize();

void lineframe(TGAImage &image, Model *model, Matrix &trans);
Vec3f objToScreen(TGAImage &image, Vec3f &objCoordinate);
Vec2f objVtToScreen(TGAImage &texture, Vec2f &vtCoordinate);
void drawTriangle(Triangle &t, TGAImage &image, TGAImage &texture,
                  float *zBuffer, float *intensity);
Matrix identity(int dimension); // 返回阶数为dimension的标准矩阵
void rasterize(Model *model, TGAImage &image, TGAImage texture, float *zBuffer,
               Vec3f &light, Matrix &trans);
void rasterizer(MyShader &shader, TGAImage &image, float *zBuffer);
Vec3f round(Vec3f vec); // 四舍五入
Matrix Viewport(int x, int y, int width,
                int height); // 将世界坐标转换到屏幕坐标的矩阵
Matrix
Projection(Vec3f eye,
           Vec3f center); // 投影矩阵，将图像由camera的位置投影到z=0的平面
inline Matrix ModelMatrix() { return identity(4); }
Matrix View(Vec3f &eye, Vec3f &center, Vec3f &up); // 进行摄像机变换的矩阵
bool withinScreen(TGAImage image,
                  Vec3f *pts) { // 判断三角形是不是在屏幕内
  int width = image.get_width(), height = image.get_height();
  return (pts[0].x >= 0 && pts[0].x <= width) &&
         (pts[0].y >= 0 && pts[0].y <= height) &&
         (pts[1].x >= 0 && pts[1].x <= width) &&
         (pts[1].y >= 0 && pts[1].y <= height) &&
         (pts[2].x >= 0 && pts[2].x <= width) &&
         (pts[2].y >= 0 && pts[2].y <= height);
}
bool withinScreen(TGAImage image,
                  Vec3i *pts) { // 判断三角形是不是在屏幕内
  int width = image.get_width(), height = image.get_height();
  return (pts[0].x >= 0 && pts[0].x <= width) &&
         (pts[0].y >= 0 && pts[0].y <= height) &&
         (pts[1].x >= 0 && pts[1].x <= width) &&
         (pts[1].y >= 0 && pts[1].y <= height) &&
         (pts[2].x >= 0 && pts[2].x <= width) &&
         (pts[2].y >= 0 && pts[2].y <= height);
}
int main(int argc, char **argv) {
  std::cerr << "commence program\n";
  // 处理命令行参数，不加参数则渲染默认的模型
  if (argc == 2) {
    model = new class Model(argv[1]);
  } else {
    model = new class Model("../obj/african_head.obj");
  }
  //

  texture.read_tga_file("../obj/african_head_diffuse.tga");
  texture.flip_vertically();
  // 初始化zBuffer
  for (
      int i = width * height; i--;
      zBuffer[i] =
          -std::numeric_limits<float>::
              max()) // 别用std::numeric_limits<float>::min()因为它表示的大于0的最小float
    ;
  Matrix model_matrix = ModelMatrix();
  Matrix view = View(camera, center, up);
  Matrix projection = Projection(camera, center);
  Matrix viewport =
      Viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

  Matrix trans = viewport * projection * view * model_matrix;
  Shader shader;
  shader.setTrans(trans);
  rasterizer(shader, image, zBuffer);
  // TGAImage frame(800, 800, TGAImage::GRAYSCALE);
  // lineframe(frame, model, trans);
  // rasterize(model, image, texture, zBuffer, light, trans);
  //  rasterize
  //  rasterizeTexture(model, image, texture, zBuffer, light);
  //  rasterizeCamera(model, image, texture, zBuffer, camera, light);
  //  dump z-buffer (debugging purposes only)
  TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      zbimage.set(i, j, TGAColor(zBuffer[i + j * width], 1));
    }
  }
  zbimage.flip_vertically(); // i want to have the origin at the left bottom
                             // corner of the image
  zbimage.write_tga_file("./images/zbuffer.tga");
  //
  std::cout << model_matrix << view << projection << viewport;
  delete model;
  return 0;
}
Matrix Viewport(int x, int y, int width,
                int height) { //(x,y)是坐标原点，width和height分别是
  Matrix viewport = identity(4);
  viewport[0][0] = width / 2.0;
  viewport[1][1] = height / 2.0;
  viewport[2][2] = depth / 2.0;
  viewport[0][3] = x + width / 2.0;
  viewport[1][3] = y + height / 2.0;
  viewport[2][3] = depth / 2.0;
  return viewport;
}
Matrix Projection(
    Vec3f eye,
    Vec3f
        center) { // 投影矩阵在homogenous坐标下可以与待投影的点无关，需要一点小小的数学技巧
  Matrix projection = identity(4);
  projection[3][2] = -1.0 / (center - eye).norm();
  return projection;
}
Vec3f round(Vec3f vec) {
  Vec3f result(int(vec.x + 0.5), int(vec.y + 0.5), int(vec.z + 0.5));
  return result;
}
Matrix View(Vec3f &eye, Vec3f &center, Vec3f &up) {
  Matrix result, translation = identity(4), rotation = identity(4);
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
  result = rotation * translation;
  return result;
}
void rasterize(Model *model, TGAImage &image, TGAImage texture, float *zBuffer,
               Vec3f &light, Matrix &trans) {

  for (int i = 0; i < model->nface(); i++) {
    Triangle t = model->getTriangle(i);
    float intensity[3];
    Vec3i screen_coordinate[3];
    for (int j = 0; j < 3; j++) {
      intensity[j] = t.normal_coordinate[j].normalize() * light;
      if (intensity[j] < 0)
        intensity[j] = 0;
      screen_coordinate[j] = (trans * v2m(t.world_coordinate[j])).m2v();
    }
    t.setScreen(screen_coordinate);
    if (withinScreen(image, screen_coordinate))
      drawTriangle(t, image, texture, zBuffer,
                   intensity); // 能渲染出五颜六色的图像=v= \o/
    std::cerr << "draw f" << i << "\n";
  }
  image.flip_vertically();
  image.write_tga_file("./images/african_head_triangle_light_back.tga");
}
