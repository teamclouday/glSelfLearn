#version 450 core

out vec2 TexCoords;

void main()
{
    const vec4 vertex[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0),
                                 vec4( 1.0, -1.0, 0.5, 1.0),
                                 vec4(-1.0,  1.0, 0.5, 1.0),
                                 vec4( 1.0,  1.0, 0.5, 1.0));

    const vec2 texCoords[] = vec2[](vec2(0.0, 1.0),
                                    vec2(1.0, 1.0),
                                    vec2(0.0, 0.0),
                                    vec2(1.0, 0.0));
    gl_Position = vertex[gl_VertexID];
    TexCoords = texCoords[gl_VertexID];
}