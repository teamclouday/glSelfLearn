#version 450 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out vec4 vs_color;

layout(std140) uniform TransformBlock
{
    float scale;
    vec3 translation;
    float rotation[3];
    mat4 projection_matrix;
} transform;

void main()
{
    gl_Position = position;
    vs_color = color;
}