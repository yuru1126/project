#include "Lambert.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

struct PS_OUT
{
    float4 Color : SV_TARGET0; // �J���[
    float4 Depth : SV_TARGET1; // ����
    float4 Normal : SV_TARGET2; // �@��
    float4 Position : SV_TARGET3; // ���W
    float4 RM : SV_TARGET4; // R+M
};

PS_OUT main(VS_OUT pin)
{
    PS_OUT output;
    output.Color = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
    float dist = length(pin.world_position.xyz - camera_position.xyz);
    output.Depth = float4(dist, pin.depthInView, dist, 1);
    output.Normal = float4(pin.normal * 0.5 + 0.5, 1);
    output.Position = float4(pin.world_position, 1);
    float roughness = 1;
    float metallic = 1; 
    output.RM = float4(roughness, metallic, 0, 1);
    return output;
}