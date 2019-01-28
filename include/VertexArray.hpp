#pragma once
#include <GL/glew.h>
#include "./VertexBuffer.hpp"
#include "./ElemBuffer.hpp"

class VertexArray
{
private:
    GLuint vaoID;
    GLuint counter;
public:
    VertexArray();
    ~VertexArray();
    void bind();
    void unbind();
    void addPointer(GLint size, GLsizei stride, uint64_t offset);
};