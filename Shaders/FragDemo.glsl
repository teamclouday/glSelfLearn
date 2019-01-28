#version 330 core
in vec2 outTexCoord;

out vec4 color;

uniform sampler2D outTexture1;
uniform sampler2D outTexture2;

void main()
{
    color = mix(texture(outTexture1, outTexCoord), texture(outTexture2, outTexCoord), 0.3);
}