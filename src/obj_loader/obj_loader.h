#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>

class ObjLoader 
{
public:
    ObjLoader();
    ~ObjLoader();

    static bool loadOBJ(
        const std::string &path, std::vector<float> &vertices, std::vector<float> &texCoords
    );
};

#endif // OBJ_LOADER_H