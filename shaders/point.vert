#version 450 core

layout (location = 0) in vec2 pos;

flat out int shape;

void main()
{
    shape = gl_VertexID;
    gl_PointSize = 100.0;
    gl_Position = vec4(pos, 1.0, 1.0);
}