// Phong Shading
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
    vec3 N;
    vec3 L;
    vec3 V;
} vs_out;

uniform vec3 lightPos = vec3(100.0);

void main()
{
    // calc view space coord
    vec4 P = mv_mat * vec4(position, 1.0);
    // calc normal in view space
    vs_out.N = mat3(mv_mat) * normal;
    // calc light vec
    vs_out.L = lightPos - P.xyz;
    // calc view vec
    vs_out.V = -P.xyz;
    gl_Position = proj_mat * P;
}