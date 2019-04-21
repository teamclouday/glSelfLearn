#version 450 core

layout (binding=0) uniform area_block
{
    uint counter_value;
};

out vec4 color;

uniform float max_area;

void main()
{
    float brightness = clamp(float(counter_value)/max_area, 0.0, 1.0);
    color = vec4(brightness, brightness, brightness, 1.0);
    // color = vec4(0.0, 0.0, 0.0, 1.0);
}