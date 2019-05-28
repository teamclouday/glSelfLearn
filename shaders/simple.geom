#version 450 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in TES_OUT
{
    vec3 N;
} geo_in[];

out GEO_OUT
{
    vec3 N;
} geo_out;

void main()
{
    geo_out.N = geo_in[0].N;
    float explode_factor = 0.5;
    for(int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position + vec4(-explode_factor * geo_in[0].N, 0.0);
        EmitVertex();
    }
    EndPrimitive();
}