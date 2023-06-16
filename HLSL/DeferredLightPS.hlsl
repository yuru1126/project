struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// ���C�g���
cbuffer CBLight : register(b1)
{
	//w=0:xyz=���� w>0:xyz�ʒu w��������
    float4 LightDirection;
    float4 LightColor; //�F
};

// �V�[���e�N�X�`��
Texture2D sceneTexture : register(t0);

Texture2D LightTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D RMTexture : register(t3);
Texture2D NormalTexture : register(t4);

// ���e�N�X�`��
Texture2D EnvTexture : register(t15);


// �V�[���T���v���X�e�[�g
SamplerState sceneSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	// �J���[�̎擾
    float4 tex = sceneTexture.Sample(sceneSampler, pin.texcoord);
	// �_��������
    float4 P = PositionTexture.Sample(sceneSampler, pin.texcoord);
    float3 PLightPos = LightDirection.xyz;
    float3 PLightDir = P.xyz - PLightPos;
	// ���C�g���
    float type = step(0.001, LightDirection.w);
	// �����̐ݒ�
    float attenuation = type < 0.1 ? 1.0 : 1.0 - (length(PLightDir) / LightDirection.w);
    attenuation = saturate(attenuation);
	// ���s��
    float3 L = lerp(LightDirection.xyz, PLightDir, type);

    float3 N = tex.xyz * 2.0 - 1.0;
    float3 C = LightColor.rgb * 2;
    C *= attenuation; //����

    L = normalize(L);
    N = normalize(N);
    float d = -dot(N, L);
    d = saturate(d); // 0�`1�ɃN���b�s���O
    tex.rgb = C * d; //���C�g����

    return tex;
}

