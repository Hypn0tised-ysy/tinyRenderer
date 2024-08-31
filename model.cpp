#include "model.h"
#include "geometry.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
Model::Model(const char *filename) : vertexes(), textures(), faces() {

  std::ifstream infile;
  infile.open(filename);
  if (infile.fail()) {
    std::cerr << "unable to open obj format file";
    return;
  }
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
      int target, iTrash, targetVt;
      std::vector<int> f;
      std::vector<int> fvt;
      iss >> trash;
      for (int i = 0; i < 3; i++) {
        iss >> target >> trash >> targetVt >> trash >> iTrash;
        f.push_back(target);
        fvt.push_back(targetVt);
      }
      faces.emplace_back(f);
      facesVt.emplace_back(fvt);
    } else if (!line.compare(0, 3, "vt ")) {
      float u, v;
      iss >> trash >> trash;
      iss >> u >> v;
      Vec2f vt(u, v);
      textures.emplace_back(vt);
    }
  }
  std::cerr << "the model has " << nface() << " faces" << " and " << nvertex()
            << " vertexes" << "\n";
}