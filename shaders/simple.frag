#version 450 core

out vec4 color;

in VS_OUT
{
    vec4 color;
} fs_int;

void main()
{
    color = fs_int.color;
}