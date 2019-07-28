#version 450 core

in vec2 uv;

out vec4 color;

uniform sampler2D tex;
uniform int mode;

vec4 hqfilter(sampler2D samp, vec2 tc);

void main()
{
    if(mode == 1)
        color = texture(tex, uv);
    else
        color = hqfilter(tex, uv);
}

vec4 hqfilter(sampler2D samp, vec2 tc)
{
    // get size of the texture
    vec2 texSize = textureSize(tex, 0);
    // scale input texture coord up, to the center of texel
    vec2 uvScaled = tc * texSize + 0.5;
    // find integer and fractional parts of texture coord
    vec2 uvInt = floor(uvScaled);
    vec2 uvFrac =fract(uvScaled);
    // replace fractional parts of the coord
    uvFrac = smoothstep(0.0, 1.0, uvFrac);
    // reassemble coord, scale back to [0.0, 1.0]
    vec2 newuv = (uvInt + uvFrac - 0.5) / texSize;
    // return result color
    return texture(samp, newuv);
}