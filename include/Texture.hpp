#pragma once
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <string>
#include <stdio.h>

class Texture
{
private:
    GLuint textureID;
public:
    Texture();
    ~Texture();
    void bind();
    void unbind();
    bool load(const std::string imageName);
};