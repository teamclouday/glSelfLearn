#version 450 core

uniform sampler2D image;

out vec4 color;

in vec2 UV;

void main()
{
    color = texture2D(image, UV);
}