#version 450 core

struct Vertex
{
    vec3 position;
    vec3 color;
};

layout (binding=0, std430) buffer my_vertices
{
    Vertex vertices[];
};

uniform mat4 transform_matrix;

out VS_OUT
{
    vec4 color;
} vs_out;

void main()
{
    gl_Position = vec4(vertices[gl_VertexID].position, 1.0);
    vs_out.color = vec4(vertices[gl_VertexID].color, 1.0);
}