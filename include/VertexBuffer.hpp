#pragma once
#include <GL/glew.h>

class VertexBuffer
{
private:
    GLuint vboID;
public:
    VertexBuffer();
    ~VertexBuffer();
    void bind();
    void unbind();
    void feed(GLsizeiptr size, GLfloat vertices[]);
};