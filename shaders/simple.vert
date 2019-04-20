#version 450 core

struct Vertex
{
    vec4 position;
    vec4 color;
};

layout (binding=0, std430) buffer my_vertices
{
    Vertex vertices[];
};

layout (binding=3, offset=8) uniform atomic_uint my_variable;

out VS_OUT
{
    vec4 color;
} vs_out;

void main()
{
    memoryBarrier();
    gl_Position = vertices[gl_VertexID].position;
    vs_out.color = vertices[gl_VertexID].color;
}