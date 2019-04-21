#version 450 core

layout (binding=0, offset=0) uniform atomic_uint area;

void main()
{
    atomicCounterIncrement(area);
}