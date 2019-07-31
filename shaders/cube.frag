#version 450 core

layout (binding = 0) uniform samplerCube tex_cubemap;

in VS_OUT
{
    vec3 tc;
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
    vec3 coord = fs_in.tc;
    // coord.z = -coord.z;
    color = texture(tex_cubemap, coord);
}