#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 mv_mat;
uniform mat4 proj_mat;

out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} vs_out;

uniform vec3 lightPos = vec3(100.0, 100.0, 100.0);

void main()
{
    vec4 P = mv_mat * vec4(position, 1.0);
    vs_out.N = mat3(mv_mat) * normal;
    vs_out.L = lightPos - P.xyz;
    vs_out.V = -P.xyz;
    gl_Position = proj_mat * P;
}