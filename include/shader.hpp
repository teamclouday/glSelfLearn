#pragma once
#include <GL/glew.h>

#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

class Shader
{
public:
    Shader(std::string vpath, std::string fpath);
    Shader(std::string vpath, std::string tcpath, std::string tepath, std::string fpath);
    Shader(std::string vpath, std::string tcpath, std::string tepath, std::string geopath, std::string fpath);
    ~Shader();
    void use();
    GLuint programID;
};