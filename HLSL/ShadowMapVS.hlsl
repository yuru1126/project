#include "ShadowMap.hlsli"

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
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
    }

    VS_OUT vout;
	// ŒõƒJƒƒ‰‚ÅÀ•W•ÏŠ·
    vout.position = mul(float4(p, 1.0f), ShadowVP);
    vout.texcoord = texcoord;
    vout.ShadowParam = vout.position;

    return vout;
}
