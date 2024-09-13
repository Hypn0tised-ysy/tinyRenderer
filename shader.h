#ifndef __SHADER_H__
#define __SHADER_H__
#include "geometry.h"
#include "tgaimage.h"
class MyShader {
public:
  virtual Vec3f vertex(int nth_face, int nth_vertex) = 0; // 进行坐标处理
  virtual bool fragment(
      Vec3f &bc,
      TGAColor &
          color) = 0; // 这个函数实际上需要在rasterizer中使用，根据返回值决定要不要渲染该点
};

class Shader : public MyShader {
public:
  Matrix trans;
  Vec2f uv[3];
  Vec3f normal[3];
  void setTrans(Matrix &m) { trans = m; };
  void setAttributes();
  void setUV(Vec2f *uv);
  void setNormal(Vec3f *normal);
  virtual Vec3f vertex(int nth_face, int nth_vertex);
  virtual bool fragment(Vec3f &bc, TGAColor &color);
};
#endif