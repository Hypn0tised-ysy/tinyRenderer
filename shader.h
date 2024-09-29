#ifndef __SHADER_H__
#define __SHADER_H__
#include "geometry.h"
#include "tgaimage.h"
void Viewport(int x, int y, int width,
              int height); // 将世界坐标转换到屏幕坐标的矩阵
void Projection(Vec3f eye,
                Vec3f center); // 投影矩阵，将图像由camera的位置投影到z=0的平面
void ModelMatrix();
void View(Vec3f &eye, Vec3f &center, Vec3f &up); // 进行摄像机变换的矩阵
void Rotation(Vec3f &eye, Vec3f &center, Vec3f &up);
class MyShader {
public:
  virtual Vec3f vertex(int nth_face, int nth_vertex) = 0; // 进行坐标处理
  virtual bool fragment(
      Vec3f &bc,
      TGAColor &
          color) = 0; // 这个函数实际上需要在rasterizer中使用，根据返回值决定要不要渲染该点
};

class GouraudShader : public MyShader {
public:
  Vec2f uv[3];
  Vec3f normal[3];
  void setAttributes();
  void setUV(Vec2f *uv);
  void setNormal(Vec3f *normal);
  virtual Vec3f vertex(int nth_face, int nth_vertex);
  virtual bool fragment(Vec3f &bc, TGAColor &color);
};
class Bling_phong_shader : public MyShader {
public:
  Bling_phong_shader(float _ks, float _kd, float _ka)
      : ks(_ks), kd(_kd), ka(_ka) {}
  void setAttributes();
  void setUV(Vec2f *uv);
  void setNormal(Vec3f *normal);
  virtual Vec3f vertex(int nth_face, int nth_vertex);
  virtual bool fragment(Vec3f &bc, TGAColor &color);
  float ks, kd, ka;
  Vec2f uv[3];
  Vec3f normal[3];
};
#endif