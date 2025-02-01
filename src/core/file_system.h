#pragma once

#include <string>

namespace Engine {

class FileSystem 
{
public:
    FileSystem();
    ~FileSystem();

    static std::string read(const std::string &filename);
};

}