// Example of sampling a sparse texture with clamped level of detail

uniform sampler2D uCommittedLodTexture;
uniform sampler2D uSparseColorTexture;

vec4 sampleFromSparse(vec2 uv)
{
    // get the available levels of detail for four texels of filtered sparse texel
    vec4 availLod = textureGather(uCommittedLodTexture, uv);
    // calc the level of detail OpenGL would sample from
    vec2 desiredLod = textureQueryLod(uSparseColorTexture, uv);
    // get maximum between two
    float maxAvailLod = max(max(availLod.x, availLod.y),
                            max(availLod.z, availLod.w));
    // calc actual LoD for sampling
    float finalLod = max(desiredLod.x, max(desiredLod.y, maxAvailLod));
    // sample from the sparse texture with the LoD
    return textureLod(uSparseColorTexture, uv, finalLod);
}