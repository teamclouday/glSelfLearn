#version 450 core

layout (quads, fractional_odd_spacing) in;

layout (binding = 0) uniform sampler2D tex_disp;
uniform mat4 mv_mat;
uniform mat4 proj_mat;
uniform float dmap_depth = 6.0;

in TCS_OUT
{
    vec2 tc;
} tes_in[];

out TES_OUT
{
    vec2 tc;
    vec3 world_coord;
    vec3 eye_coord;
} tes_out;

void main()
{
    vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
    vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
    vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
    p.y += texture(tex_disp, tc).r * dmap_depth;

    vec4 P_eye = mv_mat * p;

    tes_out.tc = tc;
    tes_out.world_coord = p.xyz;
    tes_out.eye_coord = P_eye.xyz;

    gl_Position = proj_mat * P_eye;
}