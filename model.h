/*
model类要做的事很简单
读取.obj文件中的顶点和面的数据，将其存储在model对象中
每一个model对象对应一个.obj文件的内容（很容易想到用指针对model对象进行操作）
*/
#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include "geometry.h"
class Model{
    public:
    explicit Model(const char* filename);
    ~Model(){};
    inline std::vector<int> face(int const idx){return faces[idx];}//返回第idx个面包含的顶点索引
    inline Vec3f vertex(int const idx){return vertexes[idx];}//顶点的坐标信息
    inline int nface(){return faces.size();}
    inline int nvertex(){return vertexes.size();}
    private:
    std::vector<std::vector<int>> faces;
    std::vector<Vec3f> vertexes;
};
#endif