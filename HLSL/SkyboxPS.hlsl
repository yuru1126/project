#include "Skybox.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(0,0,0,1);
	//	�J�������烏�[���h���W�ւ̃x�N�g��
    float3 E = normalize(pin.worldPosition.xyz - viewPosition.xyz);

	//	�����x�N�g���𐳋��~���}�@�ɑ�����UV���W�֕ϊ�����
    float2 texcoord;
	{
        texcoord = float2(1.0f - atan2(E.z, E.x) / 2.0f, -atan2(E.y, length(E.xz)));
        texcoord = texcoord / PI + 0.5f;
    }
	//	mipmap�g���ƕ����̂�mipmap�Ȃ��ɂȂ�悤�ɂ��Ă���
    float4 env = texture0.SampleLevel(sampler0, texcoord, 0) * pin.color;
    env.rgb = env.rgb / (50 + env.rgb);
    return env;
}