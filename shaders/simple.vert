#version 450 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;

out vec2 UV;

layout (binding = 1) uniform transform
{
    mat4 rotation;
} myTrans;

void main()
{
    gl_Position = myTrans.rotation * position;
    UV = vec2(uv.x, 1-uv.y);
}