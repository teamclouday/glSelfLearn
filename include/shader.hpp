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
    GLuint programID;
};