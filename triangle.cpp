#include "triangle.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

float getIntensity(Vec3f *objCoordinate, Vec3f &light) {
  Vec3f side1 = objCoordinate[0] - objCoordinate[1],
        side2 = objCoordinate[0] - objCoordinate[2];
  Vec3f norm = (side1 ^ side2).normalize();
  float intensity = norm * light;
  return intensity;
}
Vec3f objToScreen(TGAImage &image, Vec3f &objCoordinate) {
  int width = image.get_width();
  int height = image.get_height();
  Vec3f screen(int((objCoordinate.x + 1.0) * width / 2 + 0.5),
               int((objCoordinate.y + 1.0) * height / 2 + 0.5),
               objCoordinate.z);
  return screen;
}
Vec2f objVtToScreen(TGAImage &texture, Vec2f &vtCoordinate) {
  int width = texture.get_width();
  int height = texture.get_height();
  Vec2f uv(
      vtCoordinate.u * width,
      height -
          vtCoordinate.v *
              height); // 注意这里需要翻转y坐标，因为贴图的坐标原点在屏幕左下角
  return uv;
}
Vec3f BarycentricInterpolation(Vec3i *screen_coordinate,
                               Vec2i &point) // 求三角形的重心坐标
{
  Vec3f a = screen_coordinate[0], b = screen_coordinate[1],
        c = screen_coordinate[2];
  Vec3f v1(b.x - a.x, c.x - a.x, a.x - point.x);
  Vec3f v2(b.y - a.y, c.y - a.y, a.y - point.y);
  Vec3f bc = v1 ^ v2;
  // 因为点坐标都是int类型，所以叉乘之后的向量模一定大于等于1，如果模小于1，说明是零向量，此时abc不构成三角形
  float const EPSILON = 1e-2;
  if (std::abs(bc.z) < EPSILON)
    return Vec3f(-1, -1, -1);
  bc = bc / bc.z;
  return Vec3f(1.0 - (bc.x + bc.y), bc.x, bc.y);
}
Vec3f BarycentricInterpolation(Vec2i &point, Vec2f &a, Vec2f &b,
                               Vec2f &c) // 求三角形的重心坐标
{
  Vec3f v1(b.x - a.x, c.x - a.x, a.x - point.x);
  Vec3f v2(b.y - a.y, c.y - a.y, a.y - point.y);
  Vec3f bc = v1 ^ v2;
  // 因为点坐标都是int类型，所以叉乘之后的向量模一定大于等于1，如果模小于1，说明是零向量，此时abc不构成三角形
  float const EPSILON = 1e-2;
  if (bc.z < EPSILON)
    return Vec3f(-1, -1, -1);
  bc = bc / bc.z;
  return Vec3f(1.0 - (bc.x + bc.y), bc.x, bc.y);
}
Vec3f BarycentricInterpolation(Triangle &t, Vec2i &point) // 求三角形的重心坐标
{
  Vec2f a = Vec2f(t.screen_coordinate[0].x, t.screen_coordinate[0].y),
        b = Vec2f(t.screen_coordinate[1].x, t.screen_coordinate[1].y),
        c = Vec2f(t.screen_coordinate[2].x, t.screen_coordinate[2].y);
  return BarycentricInterpolation(point, a, b, c);
}
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
  // steep or not, judge if the line is steep to get enough samples
  bool steep = false;
  if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
    steep = true;
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int dx = x1 - x0, dy = y1 - y0;
  int error = 0;
  // 循环体中y的值和实际y的值的差，可以理解为
  // y_real-y_loop,当error大于0.5时，就需要把 y_loop+1,同时error-1
  int derror = std::abs(dy) * 2;
  int y = y0;
  const int y_add = dy > 0 ? 1 : -1;
  // for loop, set pixel's color
  if (steep) {
    for (int x = x0; x <= x1; x++) {
      image.set(y, x, color);
      error += derror;
      if (error > dx) {
        y += y_add;
        error -= 2 * dx;
      }
    }
  } else {
    for (int x = x0; x <= x1; x++) {
      image.set(x, y, color);
      error += derror;
      if (error > dx) {
        y += y_add;
        error -= 2 * dx;
      }
    }
  }
}
int getLineX(int x0, int y0, int x1, int y1,
             int y) // 给定纵坐标y，得到由（x0,y0）,(x1,y1)确定的直线的横坐标
{
  if (x0 == x1)
    return x0; // 除数不能为0啊，服了，debug才发现
  // y=kx+b, 懒得想效率更高的算法了
  float k = (y1 - y0) / (float)(x1 - x0);
  float b = y1 - k * x1;
  float x = (y - b) / k;
  float residual = x - (int)x;
  return residual > 0.5 ? (int)x : (int)x + 1; // 四舍五入
}
void drawTriangle( // 逐行扫描
    Vec2i &a, Vec2i &b, Vec2i &c, TGAImage &image,
    TGAColor color) // rasterize the triangle, fill it with the color assigned
{
  // 把三角形分成两个部分进行渲染，一行行填充颜色（因为三角形需要用两组直线来确定边界）
  // a,b,c按纵坐标从大到小排序
  if (a.y < b.y)
    std::swap(a, b);
  if (a.y < c.y)
    std::swap(a, c);
  if (b.y < c.y)
    std::swap(b, c);
  int y1 = a.y, y2 = b.y, y3 = c.y;
  int x1 = a.x, x2 = b.x, x3 = c.x;
  // 如果y1，y2，y3有两值相等，那么getlinex函数的逻辑会出问题，所以循环体不用大于等于，边界情况单独处理
  //  a到b部分的渲染
  for (int y = y1; y > y2; y--) {
    int x_1 = getLineX(x1, y1, x2, y2, y);
    int x_2 = getLineX(x1, y1, x3, y3, y);
    line(x_1, y, x_2, y, image, color);
  }
  // b到c部分的渲染
  for (int y = y2; y > y3; y--) {
    int x_1 = getLineX(x1, y1, x3, y3, y);
    int x_2 = getLineX(x2, y2, x3, y3, y);
    line(x_1, y, x_2, y, image, color);
  }
}
bool insideTriangle(Vec3i *screen_coordinate, Vec2i &point) {
  Vec3i a = screen_coordinate[0], b = screen_coordinate[1],
        c = screen_coordinate[2];
  Vec3i A(a.x, a.y, 0);
  Vec3i B(b.x, b.y, 0);
  Vec3i C(c.x, c.y, 0);
  Vec3i P(point.x, point.y, 0);
  Vec3i PA = A - P, PB = B - P, PC = C - P;
  Vec3i cross1 = (PA ^ PB);
  Vec3i cross2 = (PB ^ PC);
  Vec3i cross3 = (PC ^ PA);
  if ((cross1.z >= 0 && cross2.z >= 0 && cross3.z >= 0) ||
      (cross1.z <= 0 && cross2.z <= 0 && cross3.z <= 0)) {
    return true;
  }
  return false;
}
bool insideTriangle(int x, int y, const Vec2i &a, const Vec2i &b,
                    const Vec2i &c) {
  Vec3i A(a.x, a.y, 0);
  Vec3i B(b.x, b.y, 0);
  Vec3i C(c.x, c.y, 0);
  Vec3i P(x, y, 0);
  Vec3i PA = A - P, PB = B - P, PC = C - P;
  Vec3i cross1 = (PA ^ PB);
  Vec3i cross2 = (PB ^ PC);
  Vec3i cross3 = (PC ^ PA);
  if ((cross1.z >= 0 && cross2.z >= 0 && cross3.z >= 0) ||
      (cross1.z <= 0 && cross2.z <= 0 && cross3.z <= 0)) {
    return true;
  }
  return false;
}
bool insideTriangle(Triangle &t, int x, int y) {
  Vec2i a = Vec2i(t.screen_coordinate[0].x, t.screen_coordinate[0].y);

  Vec2i b = Vec2i(t.screen_coordinate[1].x, t.screen_coordinate[1].y);
  Vec2i c = Vec2i(t.screen_coordinate[2].x, t.screen_coordinate[2].y);
  return insideTriangle(x, y, a, b, c);
}
void drawTriangleBoundingBox( // bounding box
    Vec2i &a, Vec2i &b, Vec2i &c, TGAImage &image,
    TGAColor color) // rasterize the triangle, fill it with the color assigned
{
  // determine the bounding area
  int x1 = a.x, x2 = b.x, x3 = c.x;
  int y1 = a.y, y2 = b.y, y3 = c.y;
  if (x1 < x2)
    std::swap(x1, x2);
  if (x1 < x3)
    std::swap(x1, x3);
  if (x2 < x3)
    std::swap(x2, x3);
  if (y1 < y2)
    std::swap(y1, y2);
  if (y1 < y3)
    std::swap(y1, y3);
  if (y2 < y3)
    std::swap(y2, y3);
  int xMax = x1, xMin = x3, yMax = y1, yMin = y3;
  // iterate through the box and rasterize those pixels within the triangle
  for (int y = yMin; y <= yMax; y++) {
    for (int x = xMin; x <= xMax; x++) {
      if (insideTriangle(x, y, a, b, c))
        image.set(x, y, color);
    }
  }
}
void drawTriangleZBuffer( // bounding box
    Vec3f *pts, TGAImage &image, TGAColor color,
    float *zBuffer) // rasterize the triangle, fill it with the color assigned
{
  // determine the bounding area
  int width = image.get_width(), height = image.get_height();
  int x1 = pts[0].x, x2 = pts[1].x, x3 = pts[2].x;
  int y1 = pts[0].y, y2 = pts[1].y, y3 = pts[2].y;

  if (x1 < x2)
    std::swap(x1, x2);
  if (x1 < x3)
    std::swap(x1, x3);
  if (x2 < x3)
    std::swap(x2, x3);
  if (y1 < y2)
    std::swap(y1, y2);
  if (y1 < y3)
    std::swap(y1, y3);
  if (y2 < y3)
    std::swap(y2, y3);
  int xMax = x1, xMin = x3, yMax = y1, yMin = y3;
  // iterate through the box and rasterize those pixels within the triangle
  for (int y = yMin; y <= yMax; y++) {
    for (int x = xMin; x <= xMax; x++) {
      Vec2i a(pts[0].x, pts[0].y);
      Vec2i b(pts[1].x, pts[1].y);
      Vec2i c(pts[2].x, pts[2].y);
      if (insideTriangle(x, y, a, b, c)) {
        Vec2i point(x, y);
        Vec2f screenCoordinate[3];
        for (int i = 3; i--; screenCoordinate[i] = Vec2f(pts[i].x, pts[i].y))
          ;
        Vec3f bc =
            BarycentricInterpolation(point, screenCoordinate[0],
                                     screenCoordinate[1], screenCoordinate[2]);
        float u = bc.x, v = bc.y;
        float rz = (1 - u - v) * pts[0].z + u * pts[1].z +
                   v * pts[2].z; // bc:barycentric coordinate
        if (rz > zBuffer[x + y * width]) {
          zBuffer[x + y * width] = rz;
          image.set(x, y, color);
        }
      }
    }
  }
}
void drawTriangleTexture(Vec3f *pts, Vec2f *ptsVt, TGAImage &image,
                         TGAImage texture, float *zBuffer, float intensity) {
  int width = image.get_width(), height = image.get_height();
  int x1 = pts[0].x, x2 = pts[1].x, x3 = pts[2].x;
  int y1 = pts[0].y, y2 = pts[1].y, y3 = pts[2].y;

  if (x1 < x2)
    std::swap(x1, x2);
  if (x1 < x3)
    std::swap(x1, x3);
  if (x2 < x3)
    std::swap(x2, x3);
  if (y1 < y2)
    std::swap(y1, y2);
  if (y1 < y3)
    std::swap(y1, y3);
  if (y2 < y3)
    std::swap(y2, y3);
  int xMax = x1, xMin = x3, yMax = y1, yMin = y3;
  // iterate through the box and rasterize those pixels within the triangle
  for (int y = yMin; y <= yMax; y++) {
    for (int x = xMin; x <= xMax; x++) {
      Vec2i a(pts[0].x, pts[0].y);
      Vec2i b(pts[1].x, pts[1].y);
      Vec2i c(pts[2].x, pts[2].y);
      if (insideTriangle(x, y, a, b, c)) {
        Vec2i point(x, y);
        Vec2f screenCoordinate[3];
        for (int i = 3; i--; screenCoordinate[i] = Vec2f(pts[i].x, pts[i].y))
          ;
        Vec3f bc =
            BarycentricInterpolation(point, screenCoordinate[0],
                                     screenCoordinate[1], screenCoordinate[2]);
        float u = bc.x, v = bc.y;
        Vec2f uvCoordinate =
            (1 - u - v) * ptsVt[0] + u * ptsVt[1] + v * ptsVt[2];

        TGAColor color =
            texture.get(int(uvCoordinate.u + 0.5), int(uvCoordinate.v + 0.5));
        TGAColor colorLight(color.r * intensity, color.g * intensity,
                            color.b * intensity, 255);
        float rz = (1 - u - v) * pts[0].z + u * pts[1].z +
                   v * pts[2].z; // bc:barycentric coordinate
        if (rz > zBuffer[x + y * width]) {
          zBuffer[x + y * width] = rz;
          image.set(x, y, colorLight);
        }
      }
    }
  }
}

void rasterizeZbuffer(TGAImage &image, Model *model, Vec3f light,
                      float *zBuffer) {
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
      pts[i] = objToScreen(image, objCoordinate[i]);
    }
    intensity = getIntensity(objCoordinate, light);
    if (intensity > 0) {
      drawTriangleZBuffer(
          pts, image,
          TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255),
          zBuffer);
      std::cerr << "draw f" << i << "\n";
    }
  }
  image.flip_vertically();
  image.write_tga_file("./images/african_head_triangle_light_back.tga");
}
void drawTriangle(Triangle &t, TGAImage &image, TGAImage &texture,
                  float *zBuffer, float *intensity) {
  int width = image.get_width(), height = image.get_height();
  int x1 = t.screen_coordinate[0].x, x2 = t.screen_coordinate[1].x,
      x3 = t.screen_coordinate[2].x;
  int y1 = t.screen_coordinate[0].y, y2 = t.screen_coordinate[1].y,
      y3 = t.screen_coordinate[2].y;
  int xMax = std::max(x1, std::max(x2, x3)),
      xMin = std::min(x1, std::min(x2, x3));
  int yMax = std::max(y1, std::max(y2, y3)),
      yMin = std::min(y1, std::min(y2, y3));
  // iterate through the box and rasterize those pixels within the triangle
  for (int y = yMin; y <= yMax; y++) {
    for (int x = xMin; x <= xMax; x++) {
      if (insideTriangle(t, x, y)) {
        Vec2i point(x, y);
        Vec3f bc = BarycentricInterpolation(t, point);
        float u = bc.y, v = bc.z;
        if (bc.x < 0 || bc.y < 0 ||
            bc.z <
                0) // 这里不要用（1-u-v）<0这个条件，因为当点落在三角形边上时因为浮点数的计算误差，（1-u-v）虽然应该是0，但有时会<0
          continue;
        Vec2f uvCoordinate = (1 - u - v) * t.texture_coordinate[0] +
                             u * t.texture_coordinate[1] +
                             v * t.texture_coordinate[2];
        float r_intensity =
            (1 - u - v) * intensity[0] + u * intensity[1] + v * intensity[2];
        TGAColor color =
            texture.get(int(uvCoordinate.u + 0.5), int(uvCoordinate.v + 0.5));
        TGAColor colorLight(color.r * r_intensity, color.g * r_intensity,
                            color.b * r_intensity, 255);
        // TGAColor colorLight(r_intensity * 255, r_intensity * 255,
        //                     r_intensity * 255, 255);
        float rz = (1 - u - v) * t.screen_coordinate[0].z +
                   u * t.screen_coordinate[1].z +
                   v * t.screen_coordinate[2].z; // bc:barycentric coordinate
        if (rz > zBuffer[x + y * width]) {
          zBuffer[x + y * width] = rz;
          image.set(x, y, colorLight);
        }
      }
    }
  }
}
void lineframe(TGAImage &image, Model *model, Matrix &trans) {

  for (int i = 0; i < model->nface(); i++) {
    Triangle t = model->getTriangle(i);
    float intensity[3];
    Vec3i screen_coordinate[3];
    for (int j = 0; j < 3; j++) {
      screen_coordinate[j] =
          proj<3, 4, float>(trans * embed<4, 3, float>(t.world_coordinate[j]));
    }
    t.setScreen(screen_coordinate);

    line(screen_coordinate[0].x, screen_coordinate[0].y, screen_coordinate[1].x,
         screen_coordinate[1].y, image, TGAColor(255, 255, 255, 255));
    line(screen_coordinate[0].x, screen_coordinate[0].y, screen_coordinate[2].x,
         screen_coordinate[2].y, image, TGAColor(255, 255, 255, 255));
    line(screen_coordinate[1].x, screen_coordinate[1].y, screen_coordinate[2].x,
         screen_coordinate[2].y, image, TGAColor(255, 255, 255, 255));
    std::cerr << "draw f" << i << "\n";
  }
  image.flip_vertically();
  image.write_tga_file("./images/african_head_lineframe.tga");
}