#include "model.h"
#include "geometry.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
Model::Model(const char *filename) : vertexes(), textures(), faces() {
  // open file
  std::ifstream infile;
  infile.open(filename);
  if (infile.fail()) {
    std::cerr << "unable to open obj format file";
    return;
  }
  // read file
  std::string line;
  while (!infile.eof()) {
    char trash;
    std::getline(infile, line);
    std::istringstream iss(line);
    if (!line.compare(0, 2, "v ")) {
      float x, y, z;
      iss >> trash;
      iss >> x >> y >> z;
      Vec3f v(x, y, z);
      vertexes.emplace_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      int target, iTrash, targetVt, targetVn;
      std::vector<int> f;
      std::vector<int> fvt;
      std::vector<int> fvn;
      iss >> trash;
      for (int i = 0; i < 3; i++) {
        iss >> target >> trash >> targetVt >> trash >> targetVn;
        f.push_back(target);
        fvt.push_back(targetVt);
        fvn.push_back(targetVn);
      }
      faces.emplace_back(f);
      facesVt.emplace_back(fvt);
      facesVn.emplace_back(fvn);
    } else if (!line.compare(0, 3, "vt ")) {
      float u, v;
      iss >> trash >> trash;
      iss >> u >> v;
      Vec2f vt(u, v);
      textures.emplace_back(vt);
    } else if (!line.compare(0, 3, "vn ")) {
      float x, y, z;
      iss >> trash >> trash;
      iss >> x >> y >> z;
      Vec3f normal(x, y, z);
      normals.emplace_back(normal);
    }
  }
  //
  std::cerr << "the model has " << nface() << " faces" << " and " << nvertex()
            << " vertexes" << "\n";
}
Triangle Model::getTriangle(int const idx) {
  Triangle t;
  Vec3f world_coordinate[3];
  Vec2f texture_coordinate[3];
  Vec3f normal_coordinate[3];
  std::vector<int> face = this->face(idx);
  std::vector<int> faceVt = this->faceVt(idx);
  std::vector<int> faceVn = this->faceVn(idx);
  for (int i = 0; i < 3; i++) {
    Vec3f v = vertex(face[i] - 1);
    Vec2f vt = texture(faceVt[i] - 1);
    Vec3f vn = normal(faceVn[i] - 1);
    world_coordinate[i] = Vec3f(v.x, v.y, v.z);
    texture_coordinate[i] = Vec2f(vt.u, vt.v);
    normal_coordinate[i] = Vec3f(vn.x, vn.y, vn.z);
  }
  t.setWorld(world_coordinate);
  t.setTexture(texture_coordinate);
  t.setNormal(normal_coordinate);
  return t;
}