struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer PARTICLE_CONSTANTS : register(b0)
{
    float3 set_color;
    float min_factor;
    float max_factor;
    float3 dummy;
};

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


float4 main(VS_OUT input) : SV_TARGET0
{
    float4 color = DiffuseTexture.Sample(DecalSampler, input.texcoord) * input.color;
    color.rgb = smoothstep(min_factor, max_factor, dot(color.rgb, set_color)) * color.rgb;
    return color;
}