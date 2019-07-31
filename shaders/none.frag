#version 450 core

flat in int vsID;

layout (location = 0) out vec4 color;

void main()
{
    float r = cos(vsID);
    float g = cos(1/r);
    float b = cos(1/g);
    color = vec4(r, g, b, 1.0);
    // color = vec4(normalize(gl_FragCoord).xyz, 1.0);
}