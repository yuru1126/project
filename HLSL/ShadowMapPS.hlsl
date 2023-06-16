#include "ShadowMap.hlsli"

SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET0
{
	// Œõ‚©‚ç‚Ì‹——£‚ðƒŒƒ“ƒ_ƒŠƒ“ƒO
    float depth = pin.ShadowParam.z / pin.ShadowParam.w;
    return float4(depth, depth * depth, 0, 1);
}
