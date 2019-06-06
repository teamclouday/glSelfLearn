#version 450 core

in vec2 vTex;

layout (location = 0) uniform sampler2D hdr_image;

out vec4 oColor;

void main()
{
    float lum[25];
    vec2 size = textureSize(hdr_image, 0);
    vec2 newCoord = vec2(2.0 * gl_FragCoord.x, size.y - 1.0 - 2.0 * gl_FragCoord.y);
    vec2 tex_scale = vec2(1.0) / size;

    for(int i = 0; i < 25; i++)
    {
        vec2 tc = (newCoord + 3.5 * vec2(i % 5 - 2, i / 5 - 2));
        vec3 col = texture(hdr_image, tc * tex_scale).rgb;
        lum[i] = dot(col, vec3(0.3, 0.59, 0.11));
    }

    vec3 vColor = texelFetch(hdr_image, ivec2(newCoord), 0).rgb;

    float kernelLuminance = (
        (1.0 * (lum[0] + lum[4] + lum[20] + lum[24])) +
        (4.0 * (lum[1] + lum[3] + lum[5] + lum[9] + lum[15] + lum[19] + lum[21] + lum[23])) +
        (7.0 * (lum[2] + lum[10] + lum[14] + lum[22])) +
        (16.0 * (lum[6] + lum[8] + lum[16] + lum[18])) +
        (26.0 * (lum[7] + lum[11] + lum[13] + lum[17])) +
        (41.0 * lum[12])
    ) / 273.0;

    float exposure = sqrt(8.0 / (kernelLuminance + 0.25));

    oColor.rgb = 1.0 - exp2(-vColor * exposure);
    oColor.a = 1.0;
}