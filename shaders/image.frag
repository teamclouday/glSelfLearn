#version 450 core

layout (binding = 0) uniform sampler2D image;
layout (location = 0) out vec4 color;

in vec2 TexCoords;

void main()
{
    // float m = 1.0 + gl_FragCoord.x / 30.0;
    // vec2 s = 1.0 / textureSize(image, 0);

    // vec2 C = gl_FragCoord.xy;
    // C.y = 1800 - C.y;
    // C.x = 100 + C.x;

    // vec2 P0 = vec2(C * 2.0) + vec2(-m, -m);
    // vec2 P1 = vec2(C * 2.0) + vec2(-m,  m);
    // vec2 P2 = vec2(C * 2.0) + vec2( m, -m);
    // vec2 P3 = vec2(C * 2.0) + vec2( m , m);

    // P0 *= s;
    // P1 *= s;
    // P2 *= s;
    // P3 *= s;

    // float a = textureLod(image, P0, 0).r;
    // float b = textureLod(image, P1, 0).r;
    // float c = textureLod(image, P2, 0).r;
    // float d = textureLod(image, P3, 0).r;

    // float f = a - b - c + d;

    // m *= 2;

    // color = vec4(f) / float(m * m);
    color = texture(image, TexCoords);
}