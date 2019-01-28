#include "../include/ElemBuffer.hpp"

ElemBuffer::ElemBuffer()
{
    glGenBuffers(1, &eboID);
}

ElemBuffer::~ElemBuffer()
{
    unbind();
    glDeleteBuffers(1, &eboID);
}

void ElemBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
}

void ElemBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ElemBuffer::feed(GLsizeiptr size, GLfloat index[])
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, index, GL_STATIC_DRAW);
}