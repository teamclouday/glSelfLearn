#version 450 core

out vec4 color;

in GEO_OUT
{
    vec3 N;
} fs_in;

void main()
{
    vec3 N = normalize(fs_in.N);
    vec4 c = vec4(1.0, -1.0, 0.0, 0.0) * N.z +
             vec4(0.0, 0.0, 0.0, 1.0);
    color = clamp(c, vec4(0.0), vec4(1.0));
}