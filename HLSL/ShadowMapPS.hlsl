#include "ShadowMap.hlsli"

SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET0
{
	// ������̋����������_�����O
    float depth = pin.ShadowParam.z / pin.ShadowParam.w;
    return float4(depth, depth * depth, 0, 1);
}
