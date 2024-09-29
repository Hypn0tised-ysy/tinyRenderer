#include "rasterizer.h"
#include "geometry.h"
#include "shader.h"
#include "tgaimage.h"
#include "triangle.h"
#include <iostream>

extern Model *model;
extern Vec3f light;
bool clip(TGAImage image, Vec3i *pts);
bool clip(TGAImage image, Vec3f *pts);
Vec3f BarycentricInterpolation(Vec3i *screen_coordinate, Vec2i &point);
bool insideTriangle(Vec3i *screen_coordinate, Vec2i &point);
void rasterizer(MyShader &shader, TGAImage &image, float *zBuffer) {

  for (int i = 0; i < model->nface(); i++) {
    Vec3i screen_coordinate[3];

    for (int j = 0; j < 3; j++) {
      screen_coordinate[j] = shader.vertex(i, j);
    }

    if (clip(image, screen_coordinate))
      triangle(screen_coordinate, shader, image, zBuffer);
    std::cerr << "draw f" << i << "\n";
  }

  image.flip_vertically();
  image.write_tga_file("./images/african_head_phong.tga");
}
void triangle(Vec3i *screen_coordinate, MyShader &shader, TGAImage &image,
              float *zBuffer) {
  int width = image.get_width(), height = image.get_height();
  int x1 = screen_coordinate[0].x, x2 = screen_coordinate[1].x,
      x3 = screen_coordinate[2].x;
  int y1 = screen_coordinate[0].y, y2 = screen_coordinate[1].y,
      y3 = screen_coordinate[2].y;
  int xMax = std::max(x1, std::max(x2, x3)),
      xMin = std::min(x1, std::min(x2, x3));
  int yMax = std::max(y1, std::max(y2, y3)),
      yMin = std::min(y1, std::min(y2, y3));
  // iterate through the box and rasterize those pixels within the triangle
  for (int y = yMin; y <= yMax; y++) {
    for (int x = xMin; x <= xMax; x++) {
      Vec2i point(x, y);
      if (insideTriangle(screen_coordinate, point)) {
        Vec3f bc = BarycentricInterpolation(screen_coordinate, point);
        if (bc.x < 0 || bc.y < 0 ||
            bc.z <
                0) // 这里不要用（1-u-v）<0这个条件，因为当点落在三角形边上时因为浮点数的计算误差，（1-u-v）虽然应该是0，但有时会<0
          continue;
        float rz = bc.x * screen_coordinate[0].z +
                   bc.y * screen_coordinate[1].z +
                   bc.z * screen_coordinate[2].z;
        if (rz > zBuffer[x + y * width]) {
          TGAColor color;
          bool discard = shader.fragment(bc, color);
          if (!discard) {
            zBuffer[x + y * width] = rz;
            image.set(x, y, color);
          }
        }
      }
    }
  }
}