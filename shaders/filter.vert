#version 450 core

out vec2 uv;

void main()
{
    vec2 positions[] = vec2[](vec2(-1.0, -1.0),
                              vec2( 1.0, -1.0),
                              vec2(-1.0,  1.0),
                              vec2( 1.0,  1.0));
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    uv = positions[gl_VertexID] * vec2(0.5, -0.5) + vec2(0.5);
}