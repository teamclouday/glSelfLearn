#version 450 core

layout (binding = 0) uniform sampler2D tex;
layout (location = 0) out vec4 color;

in vec2 uv;

void main()
{
    // color = texelFetch(tex, ivec2(gl_FragCoord.x, 255 - gl_FragCoord.y), 0).xxxx;
    color = texture(tex, uv);
}