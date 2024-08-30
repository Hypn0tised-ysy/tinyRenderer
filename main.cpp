#include "geometry.h"
#include "model.h"
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
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
Model *model = NULL;
constexpr int width = 800;
constexpr int height = 800;
Vec3f objToScreen(Vec3f &objCoordinate);
void drawTriangle(
    Vec2i &a, Vec2i &b, Vec2i &c,
    TGAImage &image, // 这里TGAImage一定要传引用类型，否则画三角形画不到一张图上
    TGAColor color); // rasterize the triangle, fill it with the color assigned
void drawTriangleBoundingBox( // bounding box
    Vec2i &a, Vec2i &b, Vec2i &c, TGAImage &image,
    TGAColor color); // rasterize the triangle, fill it with the color assigned
void drawTriangleZBuffer( // bounding box
    Vec3f *pts, TGAImage &image, TGAColor color,
    float *zBuffer); // rasterize the triangle, fill it with the color assigned
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

int main(int argc, char **argv) {
  std::cerr << "commence program\n";
  // 处理命令行参数，不加参数则渲染默认的模型
  if (argc == 2) {
    model = new Model(argv[1]);
  } else {
    model = new Model("../obj/african_head.obj");
  }
  //
  Vec3f light(0, 0, 1);
  TGAImage image(width, height, TGAImage::RGB);
  // 初始化zBuffer
  float *zBuffer = new float[width * height];
  for (
      int i = width * height; i--;
      zBuffer[i] =
          -std::numeric_limits<float>::
              max()) // 别用std::numeric_limits<float>::min()因为它表示的大于0的最小float
    ;
  // 渲染图像
  for (int i = 0; i < model->nface(); i++) {
    std::vector<int> face = model->face(i);
    float intensity = 0;
    Vec3f objCoordinate[3];
    Vec3f pts[3]; // pts[0] and pts[1] stands for screen coordinate, while
                  // pts[2] is z coordinate in the obj file, pts stands for the
                  // 3 points of the
    for (int i = 0; i < 3; ++i) {
      objCoordinate[i] =
          Vec3f(model->vertex(face[i] - 1).x, model->vertex(face[i] - 1).y,
                model->vertex(face[i] - 1).z);
      pts[i] = objToScreen(objCoordinate[i]);
    }
    Vec3f side1 = objCoordinate[0] - objCoordinate[1],
          side2 = objCoordinate[0] - objCoordinate[2];
    Vec3f norm = (side1 ^ side2).normalize();
    intensity = norm * light;
    if (intensity > 0) {
      drawTriangleZBuffer(
          pts, image,
          TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255),
          zBuffer); // 能渲染出五颜六色的图像=v= \o/
      std::cerr << "draw f" << i << "\n";
    }
  }
  image.flip_vertically();
  image.write_tga_file("./images/african_head_triangle_light_back.tga");
  //
  delete model;
  return 0;
}
Vec3f objToScreen(Vec3f &objCoordinate) {
  Vec3f screen(int((objCoordinate.x + 1.0) * width / 2 + 0.5),
               int((objCoordinate.y + 1.0) * height / 2 + 0.5),
               objCoordinate.z);
  return screen;
}

/* render colorful convex
  for (int i = 0; i < model->nface(); i++) {
    std::vector<int> face = model->face(i);
    for (int j = 0; j < 3; j++) {
      Vec3f p1 = model->vertex(face[j] - 1);
      Vec3f p2 = model->vertex(face[(j + 1) % 3] - 1);
      int x0 = (p1.x + 1.0) * width / 2.0;
      int x1 = (p2.x + 1.0) * width / 2.0;
      int y0 = (p1.y + 1.0) * height / 2.0;
      int y1 = (p2.y + 1.0) * height / 2.0;
      line(x0, y0, x1, y1, image, white);
    }
    Vec2i points[3];
    for (int j = 0; j < 3; j++) {
      int x = (model->vertex(face[j] - 1).x + 1.0) * width / 2.0;
      int y = (model->vertex(face[j] - 1).y + 1.0) * height / 2.0;
      points[j] = Vec2i(x, y);
    }
    drawTriangleBoundingBox(points[0], points[1], points[2], image,
                            TGAColor(rand() % 256, rand() % 256, rand() % 256,
                                     255)); // 能渲染出五颜六色的图像=v= \o/
    std::cerr << "draw f" << i << "\n";
  }
  image.flip_vertically();
  image.write_tga_file(
      "./images/african_head_triangle_light.tga");
*/