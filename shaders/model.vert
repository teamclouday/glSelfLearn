#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 mv_mat;
uniform mat4 proj_mat;
uniform mat4 shadow_mat;
uniform vec3 lightPos;

out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec4 shadow_coord;
} vs_out;

void main()
{
    vec4 P = mv_mat * vec4(position, 1.0);
    vs_out.N = mat3(mv_mat) * normal;
    vs_out.L = lightPos - P.xyz;
    vs_out.V = -P.xyz;
    vs_out.shadow_coord = shadow_mat * vec4(position, 1.0);
    gl_Position = proj_mat * P;
}