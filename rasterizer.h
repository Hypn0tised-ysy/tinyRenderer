#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__
#include "model.h"
#include "tgaimage.h"
void rasterize(Model *model, TGAImage &image, TGAImage texture, float *zBuffer,
               Vec3f &light, Matrix &trans);
#endif