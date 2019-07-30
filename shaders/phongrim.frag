#version 450 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;

uniform vec3 diffColor = vec3(0.5);
uniform vec3 specColor = vec3(0.8);
uniform vec3 ambient = vec3(0.05);
uniform float specPower = 128.0;
uniform vec3 rimColor = vec3(1.0, 0.7, 0.4);
uniform float rimPower = 5.0;

vec3 calc_rim(vec3 N, vec3 V)
{
    // calc rim factor
    float f = 1.0 - dot(N, V);
    // constrain to [0, 1]
    f = smoothstep(0.0, 1.0, f);
    // raise to rim power
    f = pow(f, rimPower);
    return f * rimColor;
}

void main()
{
    // normalize the N, L, V
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);
    // calc R
    vec3 R = reflect(-L, N);
    // compute diffuse and specular coeff
    vec3 diffuse = max(dot(N, L), 0.0) * diffColor;
    vec3 specular = pow(max(dot(R, V), 0.0), specPower) * specColor;
    vec3 rim = calc_rim(N, V);
    color = vec4(diffuse + specular + ambient + rim, 1.0);
}