#version 450 core

out vec4 color;

layout (binding = 0) uniform sampler2D tex_star;
flat in vec4 starColor;

void main()
{
    color = starColor * texture(tex_star, gl_PointCoord);
}