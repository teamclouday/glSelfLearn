#version 450 core

layout (binding = 0) uniform sampler2D image;

out vec4 color;

in vec2 UV;

void main()
{
    // color = texture2D(image, UV);
    color = texture(image, UV);
    // color = texelFetch(image, ivec2(UV*255), 0);
}