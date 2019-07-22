#version 450 core

layout (binding = 0) uniform sampler2D sparseTex;

layout (binding = 0, std140) uniform TEXTURE_BLOCK
{
    uint foo;
};

in vec2 uv;

layout (location = 0) out vec4 color;

void main()
{
    color = textureLod(sparseTex, uv, 0.0);
}