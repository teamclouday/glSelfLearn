#version 450 core
layout (location = 0) in vec3 position;

out VS_OUT
{
    vec4 color;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 scale_matrix;

void main()
{
    gl_Position = proj_matrix * mv_matrix * scale_matrix * vec4(position, 1.0);
    vs_out.color = vec4(position, 1.0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}