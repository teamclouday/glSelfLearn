#pragma once
#include <GL/glew.h>

#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

class Shader
{
public:
    Shader();
    ~Shader();
    void add(std::string path, GLenum shaderType);
    void compile();
    void use();
    GLuint programID;
private:
    std::vector<GLuint> shaderFiles;
};