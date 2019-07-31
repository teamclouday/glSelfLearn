// no shading
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform  mat4 mv_mat;
uniform  mat4 view_mat;
uniform  mat4 proj_mat;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_ambient1;
};

uniform Material mat;

flat out int vsID;

void main()
{
    vsID = gl_VertexID;
    gl_Position = proj_mat * mv_mat * vec4(position, 1.0);
}