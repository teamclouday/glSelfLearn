#pragma once
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>

class Shader
{
public:
    // program ID
    GLuint programID;
    // constructor
    Shader(const std::string vertName, const std::string fragName);
    // constructor with geometric
    Shader(const std::string vertName, const std::string fragName, const std::string geoName);
    // use program
    void Use();
    // check program exits
    bool exits();
    // set uniforms
    void setf1(const std::string uniformName, float x);
    void setf3(const std::string uniformName, float x, float y, float z);
    void setMatrix4fv(const std::string uniformName, const GLfloat *value);
};