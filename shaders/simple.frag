#version 450 core

layout (binding = 0) uniform sampler2D image;
layout (binding = 1, rgba8i) uniform readonly iimage2D image_in;
layout (binding = 2) uniform writeonly image2D image_out;

out vec4 color;

in vec2 UV;

void main()
{
    color = texture(image, UV);

    ivec2 P = ivec2(gl_FragCoord.xy);
    ivec4 data = imageLoad(image_in, P);
    imageStore(image_out, P, -data);
}