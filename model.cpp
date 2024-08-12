#include "model.h"
#include "geometry.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
Model::Model(const char* filename):vertexes(),faces()
{

    std::ifstream infile;
    infile.open(filename);
    if(infile.fail())
    {
        std::cerr<<"unable to open obj format file";
        return;
    }
        std::string line;
    while(!infile.eof())
    {
        char trash;
        std::getline(infile,line);
        std::istringstream iss(line);
        if(!line.compare(0,2,"v "))
        {
            float x,y,z;
            iss>>trash;
            iss>>x>>y>>z;
            Vec3f v(x,y,z);
            vertexes.emplace_back(v);
        }
        else if(!line.compare(0,2,"f "))
        {
            int target,iTrash;
            std::vector<int> f;
            iss>>trash;
            for(int i=0;i<3;i++)
            {
                iss>>target>>trash>>iTrash>>trash>>iTrash;
                f.push_back(target);    
            }
            faces.emplace_back(f);
        }
    }
    std::cerr<<"the model has "<<nface()<<" faces"<<" and "<<nvertex()<<" vertexes"<<"\n";
}