#version 450 core

uniform sampler2D image;

out vec4 color;

in vec2 UV;

void main()
{
    // color = texelFetch(image, ivec2(gl_FragCoord.xy), 0);
    color = texture2D(image, UV);
    // color = vec4(1.0, 0.5, 0.3, 1.0);
}