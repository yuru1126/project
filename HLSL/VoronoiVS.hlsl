#include "Voronoi.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
)
{
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);
    vout.world_position = p;
    vout.normal = normalize(n);
    vout.binormal = float3(0.0f, 1.0f, 0.001f); //	仮の上ベクトル
    vout.binormal = normalize(vout.binormal);
    vout.tangent = normalize(cross(vout.binormal, vout.normal)); //	外積
    vout.binormal = normalize(cross(vout.normal, vout.tangent));
    vout.texcoord = texcoord;

    vout.color = color;
    vout.depthInView = vout.position.z;
    
    return vout;
}
