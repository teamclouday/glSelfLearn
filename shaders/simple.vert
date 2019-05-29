#version 450 core
layout (location = 0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out VS_OUT
{
    vec4 color0;
    vec4 color1;
} vs_out;

void main()
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    vs_out.color0 = vec4(position, 1.0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
    vs_out.color1 = vec4(0.5, 0.5, 0.5, 0.0) - vec4(position, 1.0) * 2.0;
}