#version 450 core

layout (binding = 0) uniform sampler2D hdr_image;

uniform float exposure = 1.0;

out vec4 color;

void main()
{
    vec2 size = textureSize(hdr_image, 0);
    vec2 newCoord = vec2(2*gl_FragCoord.x, size.y - 1.0 - 2*gl_FragCoord.y);
    vec4 c = texelFetch(hdr_image, ivec2(newCoord), 0);
    c.rgb = vec3(1.0) - exp(-c.rgb * exposure);
    color = c;
}