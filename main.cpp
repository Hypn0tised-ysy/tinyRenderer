#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include "triangle.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>

const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
Model *model = NULL;
const int width = 800;
const int height = 800;
void drawTriangle(
    Vec2i &a, Vec2i &b, Vec2i &c,
    TGAImage &image, // 这里TGAImage一定要传引用类型，否则画三角形画不到一张图上
    TGAColor color); // rasterize the triangle, fill it with the color assigned
void drawTriangleBoundingBox( // bounding box
    Vec2i &a, Vec2i &b, Vec2i &c, TGAImage &image,
    TGAColor color); // rasterize the triangle, fill it with the color assigned
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

int main(int argc, char **argv) {
  if (argc == 2) {
    model = new Model(argv[1]);
  } else {
    model = new Model("../obj/african_head.obj");
  }
  TGAImage image(width, height, TGAImage::RGB);
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
  Vec3f light(0, 0, 1);
  for (int i = 0; i < model->nface(); i++) {
    std::vector<int> face = model->face(i);
    float intensity = 0;
    Vec2i points[3];
    Vec3f world_coordinates[3];
    for (int j = 0; j < 3; j++) {
      int x = (model->vertex(face[j] - 1).x + 1.0) * width / 2.0;
      int y = (model->vertex(face[j] - 1).y + 1.0) * height / 2.0;
      points[j] = Vec2i(x, y);
      float rx = model->vertex(face[j] - 1).x + 1.0,
            ry = model->vertex(face[j] - 1).y + 1.0,
            rz = model->vertex(face[j] - 1).z + 1.0;
      world_coordinates[j] = Vec3f(rx, ry, rz);
    }
    Vec3f side1 = world_coordinates[0] - world_coordinates[1],
          side2 = world_coordinates[0] - world_coordinates[2];
    Vec3f norm = (side1 ^ side2).normalize();
    intensity = norm * light;
    if (intensity > 0) {

      drawTriangleBoundingBox(points[0], points[1], points[2], image,
                              TGAColor(255 * intensity, 255 * intensity,
                                       255 * intensity,
                                       255)); // 能渲染出五颜六色的图像=v= \o/
      std::cerr << "draw f" << i << "\n";
    }
  }
  image.flip_vertically();
  image.write_tga_file("./images/african_head_triangle_light_back.tga");

  delete model;
  return 0;
}