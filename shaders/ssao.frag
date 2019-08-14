#version 450 core

layout (binding = 0) uniform sampler2D sColor;
layout (binding = 1) uniform sampler2D sNormalDepth;

layout (location = 0) out vec4 color;

uniform float ssao_level = 1.0;
uniform float object_level = 0.3;
uniform float ssao_radius = 5.0;
uniform bool weight_by_angle = true;
uniform uint point_count = 10;
uniform bool randomize_points = true;

layout (binding = 0, std140) uniform SAMPLE_POINTS
{
    vec4 pos[256];
    vec4 random_vectors[256];
} points;

void main()
{
    // texture pos
    vec2 P = gl_FragCoord.xy / textureSize(sNormalDepth, 0);
    // normal and depth
    vec4 ND = textureLod(sNormalDepth, P, 0);
    // extract normal and depth
    vec3 N = ND.xyz;
    float myDepth = ND.w;

    int i;
    int j;
    int n;

    float occ = 0.0;
    float total = 0.0;

    // n is pseudo-random number
    n = (int(gl_FragCoord.x * 7123.2315 + 125.232) *
         int(gl_FragCoord.y * 3137.1519 + 234.8)) ^
         int(myDepth);
    // pull one of random vectors
    vec4 v = points.random_vectors[n & 256];

    // r is radius randomizer
    float r = (v.r + 3.0) * 0.1;
    if(!randomize_points)
        r = 0.5;
    
    for(i = 0; i < point_count; i++)
    {
        // get direction
        vec3 dir = points.pos[i].xyz;
        // put into correct hemisphere
        if(dot(N, dir) < 0.0)
            dir = -dir;
        // f is distance stopped in this direction
        // z is interpolated depth
        float f = 0.0;
        float z = myDepth;
        // take 4 steps
        total += 4.0;
        for(j = 0; j < 4; j++)
        {
            // step in right direction
            f += r;
            // step towards viewer reduces z
            z -= dir.z * f;
            // read depth from current fragment
            float their_depth = textureLod(sNormalDepth, (P + dir.xy*f*ssao_radius), 0).w;
            // calc a weighting for this fragment's contribution to occlusion
            float d = abs(their_depth - myDepth);
            d *= d;
            // if obscured, accumulate occlusion
            if((z - their_depth) > 0.0)
                occ += 4.0 / (1.0 + d);
        }
    }
    // calc occlusion amount
    float ao_amount = (1.0 - occ / total);
    // get object color
    vec4 object_color = textureLod(sColor, P, 0);
    // mix in ambient color scaled by SSAO level
    color = object_level * object_color + mix(vec4(0.2), vec4(ao_amount), ssao_level);
}