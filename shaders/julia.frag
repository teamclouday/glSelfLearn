#version 450 core

in vec2 initial_z;

out vec4 color;

uniform sampler1D tex_grad;
uniform vec2 C;

void main()
{
    int iterations = 0;
    vec2 Z = initial_z;
    const float threshold_squared = 16.0;
    const int max_iterations = 256;
    while(iterations < max_iterations && dot(Z, Z) < threshold_squared)
    {
        vec2 Z_squared;
        Z_squared.x = Z.x * Z.x - Z.y * Z.y;
        Z_squared.y = 2.0 * Z.x * Z.y;
        Z = Z_squared + C;
        iterations++;
    }
    if(iterations == max_iterations)
        color = vec4(0.0, 0.0, 0.0, 1.0);
    else
        color = texture(tex_grad, float(iterations) / float(max_iterations));
}