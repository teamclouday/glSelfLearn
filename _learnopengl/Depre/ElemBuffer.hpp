#pragma once
#include <GL/glew.h>

class ElemBuffer
{
private:
    GLuint eboID;
public:
    ElemBuffer();
    ~ElemBuffer();
    void bind();
    void unbind();
    void feed(GLsizeiptr size, GLfloat index[]);
};