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
    // use program
    void Use();
    // check program exits
    bool Exits();
};