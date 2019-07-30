// Environmental Mapping
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform  mat4 mv_mat;
uniform  mat4 view_mat;
uniform  mat4 proj_mat;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_ambient1;
};

uniform Material mat;

out VS_OUT
{
    vec3 normal;
    vec3 view;
} vs_out;

void main()
{
    vec4 pos_vs = mv_mat * vec4(position, 1.0);
    vs_out.normal = mat3(mv_mat) * normal;
    vs_out.view = pos_vs.xyz;
    gl_Position = proj_mat * pos_vs;
}