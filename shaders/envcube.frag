#version 450 core

layout (binding = 0) uniform samplerCube tex_cubemap;

in VS_OUT
{
    vec3 normal;
    vec3 view;
} fs_in;

layout (location = 0) out vec4 color;

uniform int indicator;
const float ratio = 1.0 / 2.42;

void main()
{
    vec3 r = vec3(1.0);
    if(indicator > 0)
    {
        r = reflect(fs_in.view, normalize(fs_in.normal));
    }
    else if(indicator < 0)
    {
        r = refract(fs_in.view, normalize(fs_in.normal), ratio);
    }
    color = vec4(texture(tex_cubemap, r).rgb, 1.0);
}