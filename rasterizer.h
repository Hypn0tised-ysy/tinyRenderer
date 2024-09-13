#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__
#include "geometry.h"
#include "model.h"
#include "shader.h"
#include "tgaimage.h"
void rasterizer(MyShader &shader, TGAImage &image, float *zbuffer);
void triangle(Vec3i *screen_coordinate, MyShader &shader, TGAImage &image,
              float *zbuffer);
#endif