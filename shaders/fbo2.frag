#version 450 core

uniform sampler2D tex;

in VS_OUT
{
    vec4 color;
    vec2 texCoord;
} fs_in;

out vec4 color;

void main()
{
    color = mix(fs_in.color, texture(tex, fs_in.texCoord), 0.7);
}