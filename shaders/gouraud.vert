// Gouraud Shading
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

uniform vec3 lightPos = vec3(100.0);
uniform float specPower = 128.0;
uniform vec3 diffuseColor = vec3(0.5);
uniform vec3 specColor = vec3(0.8);
uniform vec3 ambient = vec3(0.05);

out VS_OUT
{
    vec3 color;
} vs_out;

void main()
{
    // calc view-space coord
    vec4 P = mv_mat * vec4(position, 1.0);
    // calc normal in view space
    vec3 N = mat3(mv_mat) * normal;
    // calc view-space light vec
    vec3 L = lightPos - P.xyz;
    // calc view vec
    vec3 V = -P.xyz;
    // normalize
    N = normalize(N);
    L = normalize(L);
    V = normalize(V);
    // calc R by reflecting -L around plane for N
    vec3 R = reflect(-L, N);
    // calc diffuse and specular distributions
    vec3 diffuse = max(dot(N, L), 0.0) * diffuseColor;
    vec3 specular = pow(max(dot(R, V), 0.0), specPower) * specColor;
    vs_out.color = ambient + diffuse + specular;
    gl_Position = proj_mat * P;
}