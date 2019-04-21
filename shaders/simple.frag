#version 450 core

layout (binding=0, offset=0) uniform atomic_uint atom;

out vec4 color;

void main()
{
    uint counter = atomicCounterIncrement(atom);
    float r = (float(counter) / 255.0) / 255.0 / 2.0;
    color = vec4(r, r, r, 1.0);
}