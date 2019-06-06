#version 450 core

layout (binding = 0) uniform sampler2D hdr_image;

out vec4 color;

void main()
{
    vec2 newCoord = vec2(gl_FragCoord.x, 1.0 - gl_FragCoord.y);
    color = texture(hdr_image, 2.0 * newCoord / textureSize(hdr_image, 0));
    // color = vec4(0.2, 0.8, 0.2, 1.0);
}