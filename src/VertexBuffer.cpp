#include "../include/VertexBuffer.hpp"

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &vboID);
}

VertexBuffer::~VertexBuffer()
{
    unbind();
    glDeleteBuffers(1, &vboID);
}

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
}

void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::feed(GLsizeiptr size, GLfloat vertices[])
{
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}