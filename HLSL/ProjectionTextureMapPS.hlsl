#include "ProjectionTextureMap.hlsli"

SamplerState sampler_states : register(s0);
SamplerState sampler_states2 : register(s1);

Texture2D MainTexture : register(t0);
Texture2D DepthTexture : register(t4);
Texture2D ColorTexture : register(t5);
Texture2D textureMap : register(t6);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = MainTexture.Sample(sampler_states, pin.texcoord);
    float sceneDepth = DepthTexture.Sample(sampler_states, pin.texcoord).r;
    
    float4 ndc_position;
    
	//テクスチャ空間からNDCへ
    ndc_position.x = pin.texcoord.x * +2 - 1;
    ndc_position.y = pin.texcoord.y * -2 + 1;
    ndc_position.z = sceneDepth;
    ndc_position.w = 1;
    
    //NDC空間からworldへ
    float4 world_position = mul(ndc_position, InverseViewProjection);
    
    world_position = world_position / world_position.w;
    
    float4 projection_texture_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    if (CircleFlag)
    {
        float4 projection_texture_position_0 = mul(world_position, CircleTransform);
        projection_texture_position_0 /= projection_texture_position_0.w;
        projection_texture_position_0.x = projection_texture_position_0.x * 0.5 + 0.5;
        projection_texture_position_0.y = -projection_texture_position_0.y * 0.5 + 0.5;
        float4 projection_texture_color = textureMap.Sample(sampler_states2, projection_texture_position_0.xy);
        
        color.rgb += projection_texture_color.rgb * projection_texture_color.a *
        projection_texture_intensity.rgb * projection_texture_intensity.a;  
        
    }
    
    return color;
}