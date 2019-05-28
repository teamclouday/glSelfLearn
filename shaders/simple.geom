#version 450 core

layout (triangles, invocations = 4) in;
layout (triangle_strip, max_vertices = 8) out;

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
    float explode_factor = 1;
    for(int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position + vec4(-explode_factor * geo_in[0].N, 0.0);
        gl_ViewportIndex = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}