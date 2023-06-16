#include "Particle.hlsli"

SamplerState sampler_states : register(s0);
Texture2D texture_maps[3] : register(t0);
float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = float4(1, 0, 0, 1);
    if (pin.draw_type == 0)
    {
        color = texture_maps[0].Sample(sampler_states, pin.texcoord) * pin.color;
    }
    if (pin.draw_type == 1)
    {
        color = texture_maps[1].Sample(sampler_states, pin.texcoord) * pin.color;
    }
    if (pin.draw_type == 2)
    {
        color = texture_maps[2].Sample(sampler_states, pin.texcoord) * pin.color;
    }
    
    return color;
}