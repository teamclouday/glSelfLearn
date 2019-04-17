#version 450 core
layout (location = 0) in vec4 position;

// out vec4 vs_color;

void main()
{
    gl_Position = position;
}