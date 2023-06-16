struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// �V�[���e�N�X�`��
Texture2D sceneTexture : register(t0);

// �V�[���T���v���X�e�[�g
SamplerState sceneSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = 0;
    float2 adjust;
    float sum = 0;
    for (int y = -3; y < 3; y++)
    {
        adjust.y = y;
        for (int x = -3; x < 3; x++)
        {
            adjust.x = x;
            float rate = 1.0 - (length(adjust) / 3);
            rate = saturate(rate);
            float2 tex = sceneTexture.Sample(sceneSampler, pin.texcoord + adjust * 0.003).xy;
            color.xy += tex * rate;
            sum += rate;
        }
    }
    color /= sum;
    color.a = 1;

    return color;
}