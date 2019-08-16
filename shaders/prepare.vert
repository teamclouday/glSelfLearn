#version 450 core

out VS_OUT
{
    vec3 ray_origin;
    vec3 ray_direction;
} vs_out;

uniform vec3 ray_origin;
uniform mat4 ray_lookat;
uniform float aspect = 0.75;

uniform vec3 direction_scale = vec3(1.9, 1.9, 1.0);
uniform vec3 direction_bias = vec3(0.0, 0.0, 0.0);

// layout (binding = 0, std140) uniform UNIFORM_BLOCK
// {
//     mat4 mv_mat;
//     mat4 view_mat;
//     mat4 proj_mat;
// };

void main()
{
    const vec4[] vertices = vec4[](vec4(-1.0, -1.0, 1.0, 1.0),
                                   vec4( 1.0, -1.0, 1.0, 1.0),
                                   vec4(-1.0,  1.0, 1.0, 1.0),
                                   vec4( 1.0,  1.0, 1.0, 1.0));
    // const vec3[36] vertices = vec3[36](vec3(-1.0,  1.0, -1.0),
    //                                    vec3(-1.0, -1.0, -1.0),
    //                                    vec3( 1.0, -1.0, -1.0),
    //                                    vec3( 1.0, -1.0, -1.0),
    //                                    vec3( 1.0,  1.0, -1.0),
    //                                    vec3(-1.0,  1.0, -1.0),

    //                                    vec3(-1.0, -1.0,  1.0),
    //                                    vec3(-1.0, -1.0, -1.0),
    //                                    vec3(-1.0,  1.0, -1.0),
    //                                    vec3(-1.0,  1.0, -1.0),
    //                                    vec3(-1.0,  1.0,  1.0),
    //                                    vec3(-1.0, -1.0,  1.0),

    //                                    vec3(1.0, -1.0, -1.0),
    //                                    vec3(1.0, -1.0,  1.0),
    //                                    vec3(1.0,  1.0,  1.0),
    //                                    vec3(1.0,  1.0,  1.0),
    //                                    vec3(1.0,  1.0, -1.0),
    //                                    vec3(1.0, -1.0, -1.0),

    //                                    vec3(-1.0, -1.0,  1.0),
    //                                    vec3(-1.0,  1.0,  1.0),
    //                                    vec3( 1.0,  1.0,  1.0),
    //                                    vec3( 1.0,  1.0,  1.0),
    //                                    vec3( 1.0, -1.0,  1.0),
    //                                    vec3(-1.0, -1.0,  1.0),

    //                                    vec3(-1.0,  1.0, -1.0),
    //                                    vec3( 1.0,  1.0, -1.0),
    //                                    vec3( 1.0,  1.0,  1.0),
    //                                    vec3( 1.0,  1.0,  1.0),
    //                                    vec3(-1.0,  1.0,  1.0),
    //                                    vec3(-1.0,  1.0, -1.0),

    //                                    vec3(-1.0, -1.0, -1.0),
    //                                    vec3(-1.0, -1.0,  1.0),
    //                                    vec3( 1.0, -1.0, -1.0),
    //                                    vec3( 1.0, -1.0, -1.0),
    //                                    vec3(-1.0, -1.0,  1.0),
    //                                    vec3( 1.0, -1.0,  1.0));
    vec4 pos = vertices[gl_VertexID];
    gl_Position = pos;
    vs_out.ray_origin = ray_origin * vec3(1.0, 1.0, -1.0);
    vs_out.ray_direction = (ray_lookat * vec4(pos.xyz * direction_scale * vec3(1.0, aspect, 2.0) + direction_bias, 0.0)).xyz;
    // vs_out.ray_direction = mat3(ray_lookat) * vec3(pos.xyz * direction_scale * vec3(1.0, aspect, 2.0) + direction_bias);
    // vs_out.ray_direction = pos.xyz;
}