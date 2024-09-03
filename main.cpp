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
constexpr int depth = 255; // 把z坐标投影到[0,255]，输出z-buffer图像以供调试

Vec3f objToScreen(TGAImage &image, Vec3f &objCoordinate);
Vec2f objVtToScreen(TGAImage &texture, Vec2f &vtCoordinate);
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
void drawTriangleTexture(Vec3f *pts, Vec2f *ptsVt, TGAImage &image,
                         TGAImage texture, float *zBuffer, float intensity);
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
float getIntensity(Vec3f *objCoordinate, Vec3f &light);
Matrix identity(int dimension); // 返回阶数为dimension的标准矩阵
void rasterizeZbuffer(TGAImage &image, Model *model, Vec3f light,
                      float *zBuffer);
void rasterizeTexture(Model *model, TGAImage &image, TGAImage texture,
                      float *zBuffer, Vec3f &light); // 渲染图像
void rasterizeTexture(Model *model, TGAImage &image, TGAImage texture,
                      float *zBuffer);
void rasterizeCamera(Model *model, TGAImage &image, TGAImage texture,
                     float *zBuffer, Vec3f &camera, Vec3f &light);
Vec3f round(Vec3f vec); // 四舍五入
Matrix viewport(int x, int y, int width,
                int height); // 将世界坐标转换到屏幕坐标的矩阵
Matrix
projection(Vec3f camera); // 投影矩阵，将图像由camera的位置投影到z=0的平面
inline bool withinScreen(TGAImage image,
                         Vec3f *pts) { // 判断三角形是不是在屏幕内
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
    model = new Model(argv[1]);
  } else {
    model = new Model("../obj/african_head.obj");
  }
  //
  Vec3f camera(0, 0, 3);
  Vec3f light(0, 0, 1);
  TGAImage image(width, height, TGAImage::RGB);
  // texture
  TGAImage texture;
  texture.read_tga_file("../obj/african_head_diffuse.tga");
  // 初始化zBuffer
  float *zBuffer = new float[width * height];
  for (
      int i = width * height; i--;
      zBuffer[i] =
          -std::numeric_limits<float>::
              max()) // 别用std::numeric_limits<float>::min()因为它表示的大于0的最小float
    ;
  // rasterize
  // rasterizeTexture(model, image, texture, zBuffer, light);
  rasterizeCamera(model, image, texture, zBuffer, camera, light);
  // dump z-buffer (debugging purposes only)
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
  delete model;
  return 0;
}
Matrix viewport(int x, int y, int width,
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
Matrix projection(
    Vec3f
        camera) { // 投影矩阵在homogenous坐标下可以与待投影的点无关，需要一点小小的数学技巧
  Matrix projection = identity(4);
  projection[3][2] = -1.0 / camera.z;
  return projection;
}
void rasterizeCamera(Model *model, TGAImage &image, TGAImage texture,
                     float *zBuffer, Vec3f &camera,
                     Vec3f &light) { // 渲染图像,不加光照的版本
  for (int i = 0; i < model->nface(); i++) {
    std::vector<int> face = model->face(i);
    std::vector<int> faceVt = model->faceVt(i);
    float intensity = 0;
    Vec3f objCoordinate[3];
    Vec2f vtCoordinate[3];
    Vec3f pts[3]; // pts[0] and pts[1] stands for screen coordinate, while
    // pts[2] is z coordinate in the obj file, pts stands for the
    // 3 points of the
    Vec2f ptsVt[3];
    for (int i = 0; i < 3; ++i) {
      objCoordinate[i] =
          Vec3f(model->vertex(face[i] - 1).x, model->vertex(face[i] - 1).y,
                model->vertex(face[i] - 1).z);
      vtCoordinate[i] = Vec2f(model->texture(faceVt[i] - 1).u,
                              model->texture(faceVt[i] - 1).v);
      Matrix proj = projection(camera);
      Matrix view =
          viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
      pts[i] = round((view * proj * v2m(objCoordinate[i])).m2v());

      ptsVt[i] = objVtToScreen(texture, vtCoordinate[i]);
    }
    if (withinScreen(image, pts))
      drawTriangleTexture(pts, ptsVt, image, texture, zBuffer,
                          1.0); // 能渲染出五颜六色的图像=v= \o/
    std::cerr << "draw f" << i << "\n";
  }
  image.flip_vertically();
  image.write_tga_file("./images/african_head_triangle_light_back.tga");
}
Vec3f round(Vec3f vec) {
  Vec3f result(int(vec.x + 0.5), int(vec.y + 0.5), int(vec.z + 0.5));
  return result;
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