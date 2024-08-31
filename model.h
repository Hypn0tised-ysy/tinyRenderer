/*
model类要做的事很简单
读取.obj文件中的顶点和面的数据，将其存储在model对象中
每一个model对象对应一个.obj文件的内容（很容易想到用指针对model对象进行操作）
*/
#ifndef __MODEL_H__
#define __MODEL_H__
#include "geometry.h"
#include <cstddef>
#include <vector>

class Model {
public:
  explicit Model(const char *filename);
  ~Model(){};
  inline std::vector<int> face(int const idx) {
    return faces[idx];
  } // 返回第idx个面包含的顶点索引
  inline std::vector<int> faceVt(int const idx) { return facesVt[idx]; }
  inline Vec3f vertex(int const idx) { return vertexes[idx]; } // 顶点的坐标信息
  inline Vec2f texture(int const idx) {
    return textures[idx];
  } // 返回索引为idx的vt坐标的u，v信息
  inline int nface() { return faces.size(); } // 返回面的数量
  inline int nfaceVt() { return facesVt.size(); }
  inline int nvertex() { return vertexes.size(); }
  inline int ntextures() { return textures.size(); }

private:
  std::vector<std::vector<int>> faces;
  std::vector<std::vector<int>> facesVt;
  std::vector<Vec3f> vertexes;
  std::vector<Vec2f> textures;
};
#endif