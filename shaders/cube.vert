#version 450 core

out VS_OUT
{
    vec3 tc;
} vs_out;

uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
    vec3[4] vertices = vec3[4](vec3(-1.0, -1.0, -1.0),
                               vec3( 1.0, -1.0, -1.0),
                               vec3(-1.0,  1.0, -1.0),
                               vec3( 1.0,  1.0, -1.0));
    // vec3[14] vertices = vec3[14](vec3(-1.0,  1.0,  1.0),
    //                             vec3( 1.0,  1.0,  1.0),
    //                             vec3(-1.0, -1.0,  1.0),
    //                             vec3( 1.0, -1.0,  1.0),
    //                             vec3( 1.0, -1.0, -1.0),
    //                             vec3( 1.0,  1.0,  1.0),
    //                             vec3( 1.0,  1.0, -1.0),
    //                             vec3(-1.0,  1.0,  1.0),
    //                             vec3(-1.0,  1.0, -1.0),
    //                             vec3(-1.0, -1.0,  1.0),
    //                             vec3(-1.0, -1.0, -1.0),
    //                             vec3( 1.0, -1.0, -1.0),
    //                             vec3(-1.0,  1.0, -1.0),
    //                             vec3( 1.0,  1.0, -1.0));
    vs_out.tc = mat3(view_mat) * vertices[gl_VertexID];
    gl_Position = vec4(vertices[gl_VertexID], 1.0);
}