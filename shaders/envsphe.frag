#version 450 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec3 normal;
    vec3 view;
} fs_in;

layout (binding = 0) uniform sampler2D tex_envmap;

void main()
{
    vec3 u = normalize(fs_in.view);
    vec3 r = reflect(u, normalize(fs_in.normal));
    // comp scale factor
    r.z += 1.0;
    float m = 0.5 * inversesqrt(dot(r, r));
    // sample from scaled and biased texture coord
    color = texture(tex_envmap, r.xy * m + vec2(0.5));
}