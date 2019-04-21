#pragma once
#include <GL/glew.h>

#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#include "myerror.hpp"

class Shader
{
public:
    Shader();
    ~Shader();
    void add(std::string path, GLenum shaderType);
    void compile();
    void use();
    void disuse();
    GLuint programID;
private:
    std::vector<GLuint> shaderFiles;
};