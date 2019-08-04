#version 450 core

layout (location = 0) out vec4 color;
layout (binding = 0) uniform sampler2DShadow shadow_tex;

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec4 shadow_coord;
} fs_in;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_ambient1;
};

uniform Material mat;

uniform vec3 diffColor = vec3(0.5);
uniform vec3 specColor = vec3(0.8);
uniform vec3 ambient = vec3(0.05);
uniform float specPower = 128.0;

uniform vec3 lightColor;
uniform bool full_shading = true;

void main()
{
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);
    vec3 R = reflect(-L, N);
    vec3 diffuse = max(dot(N, L), 0.0) * diffColor;
    vec3 specular = pow(max(dot(R, V), 0.0), specPower) * specColor;
    color = textureProj(shadow_tex, fs_in.shadow_coord) * mix(vec4(1.0), vec4(lightColor * (diffuse + specular + ambient), 1.0), bvec4(full_shading));
    // color = vec4(lightColor * (diffuse + specular + ambient), 1.0);
}