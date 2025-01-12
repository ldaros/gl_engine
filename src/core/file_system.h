#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>

class FileSystem 
{
public:
    FileSystem();
    ~FileSystem();

    static std::string read(const std::string &filename);
};

#endif // FILE_MANAGER_H