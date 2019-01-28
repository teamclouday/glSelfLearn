#include "../include/VertexArray.hpp"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &vaoID);
    counter = 0;
}

VertexArray::~VertexArray()
{
    unbind();
    glDeleteVertexArrays(1, &vaoID);
}

void VertexArray::bind()
{
    glBindVertexArray(vaoID);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::addPointer(GLint size, GLsizei stride, uint64_t offset)
{
    glVertexAttribPointer(counter, size, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    glEnableVertexAttribArray(counter);
    counter++;
}