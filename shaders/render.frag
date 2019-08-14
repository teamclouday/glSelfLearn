#version 450 core

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal_depth;

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;

uniform vec3 diffColor = vec3(0.8, 0.8, 0.8);
uniform vec3 specColor = vec3(0.01);
uniform float specPow = 128.0;
uniform float shadingLevel = 0.7;

void main()
{
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);
    vec3 R = reflect(-L, N);
    vec3 diff = max(dot(N, L), 0.0) * diffColor;
    diff *= diff;
    vec3 spec = pow(max(dot(R, V), 0.0), specPow) * specColor;
    color = mix(vec4(0.0), vec4(diff + spec, 1.0), shadingLevel);
    normal_depth = vec4(N, fs_in.V.z);
}