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
TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
// texture
TGAImage texture;
float *zBuffer = new float[width * height];
Vec3f camera(1, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f light = Vec3f(1, -1, 1).normalize();

void lineframe(TGAImage &image, Model *model, Matrix &trans);
void drawTriangle(Triangle &t, TGAImage &image, TGAImage &texture,
                  float *zBuffer, float *intensity);
void rasterizer(MyShader &shader, TGAImage &image, float *zBuffer);

bool clip(TGAImage image,
          Vec3f *pts) { // 判断三角形是不是在屏幕内
  int width = image.get_width(), height = image.get_height();
  return (pts[0].x >= 0 && pts[0].x <= width) &&
         (pts[0].y >= 0 && pts[0].y <= height) &&
         (pts[1].x >= 0 && pts[1].x <= width) &&
         (pts[1].y >= 0 && pts[1].y <= height) &&
         (pts[2].x >= 0 && pts[2].x <= width) &&
         (pts[2].y >= 0 && pts[2].y <= height);
}
bool clip(TGAImage image,
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
  ModelMatrix();
  View(camera, center, up);
  Projection(camera, center);
  Viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
  GouraudShader shader;
  rasterizer(shader, image, zBuffer);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      zbuffer_image.set(i, j, TGAColor(zBuffer[i + j * width], 1));
    }
  }
  zbuffer_image.flip_vertically(); // i want to have the origin at the left
                                   // bottom corner of the image
  delete model;
  return 0;
}
