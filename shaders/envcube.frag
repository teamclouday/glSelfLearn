#version 450 core

layout (binding = 0) uniform samplerCube tex_cubemap;

in VS_OUT
{
    vec3 normal;
    vec3 view;
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
    vec3 r = reflect(fs_in.view, normalize(fs_in.normal));
    color = texture(tex_cubemap, r);
}