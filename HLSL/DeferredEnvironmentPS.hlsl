// HDRI(�p�m���}�摜)�̉f�荞��
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CbScene : register(b0)
{
    float4 viewPosition;
    row_major float4x4 viewProjection;
};


//	�e�N�X�`��
Texture2D MainTexture : register(t0);
Texture2D LightTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D RMTexture : register(t3);
// SSR
Texture2D DepthTexture : register(t4);
Texture2D ColorTexture : register(t5);


Texture2D EnvTexture : register(t15);

// �V�[���T���v���X�e�[�g
SamplerState sceneSampler : register(s0);
SamplerState ClampSampler : register(s1);

// SSR
// �f�肱�މ�ʃe�N�X�`����UV��Ԃ�
// P : ���W�@R:�����̔���
float2 SSR(float3 P, float3 R)
{
	// ���˃x�N�g��(�����Ɩ@���Ŕ���)
    R = normalize(R);
	// ������Ƃ��i�߂�
    R *= 0.3; //0.1 = ���Ői�߂鋗��
	[loop] //Loop�̂܂܂ŗǂ���B
    for (int i = 0; i < 100; i++)
    {
        P += R; //�i�߂�
		// �����ɂԂ������H
		// P�܂ł̋�����
        float DistP = length(P - viewPosition.xyz);
		// �����_�����O���ʂ̋����Ŕ�r
		// P�̃X�N���[�����W �� UV �� Depth���狗���擾
        float4 ScreenP = mul(float4(P, 1.0), viewProjection);
		// �X�N���[�����W(-1 <--> 1) �� UV(0<-->1)
        float2 Puv = (ScreenP.xy / ScreenP.w) * 0.5 + 0.5;
        Puv.y = 1.0 - Puv.y; //Y���]
		// ��ʊO����
        if (Puv.x < 0) return -1;
        if (Puv.y < 0) return -1;
        if (Puv.x > 1) return -1;
        if (Puv.y > 1) return -1;
							 		 
		// Depth���狗���擾
        float DistS = DepthTexture.Sample(
			sceneSampler, Puv).x;

		// ������r(�Q�ȏ�߂��ꍇ��NG)
        if (DistP > DistS && DistP - 2 < DistS)
        {
			// ���� = �f�荞�݃|�C���g
            return Puv;
        }
    }
    return -1; // �f�荞�ݖ�
}

float4 main(VS_OUT pin) : SV_TARGET
{
	// �@���̎擾
    float4 tex = MainTexture.Sample(sceneSampler, pin.texcoord);
    float3 orgN = tex.xyz * 2.0 - 1.0;
    float3 N = normalize(orgN);

	// �����̔���
    float4 P = PositionTexture.Sample(sceneSampler, pin.texcoord);
    float3 E = P.xyz - viewPosition.xyz; //����
    float3 R = reflect(normalize(E), N);
	// R���㉺�ƕ����ɕ���
    float2 v;
    v.y = R.y; //Y�͂��̂܂�
    float3 work = R;
    work.y = 0; //��������
    work = normalize(work); //���p�̂�
    v.x = work.x;
	//�Ђ��݂𒼂�

    float2 uv = v;
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
    uv.x *= 0.5;
	// �����
    if (R.z > 0)
    {
        uv.x = 1.0 - uv.x;
    }
	//Roughness���l���Ă݂�
    float4 RM = RMTexture.Sample(sceneSampler, pin.texcoord);
    float roughness = RM.x;
	//roughness = 1;
	//r 0.0  0.3   0.5  1.0 
	//l 0.0  8.0  10.0  11.0
    float w = 1.0 - pow(1.0 - roughness, 5.0);
    float level = w * 12.0;
    float4 env = EnvTexture.SampleLevel(ClampSampler, uv, level);
	//�P���ȃg�[���}�b�s���O
    env.rgb = env.rgb / (100 + env.rgb);

	// SSR �f�荞�݂�UV���󂯎��
    float2 ssr = SSR(P.xyz, R);
    if (ssr.x >= 0)
    {
		// Mipmap�Ή�
        float level = roughness * 5.0;
        float4 texR = ColorTexture.SampleLevel(
					sceneSampler, ssr, level);
		// �u�����h
        float rate = roughness;
        rate = rate * 0.6 + 0.4; //0.4---1.0
        env = lerp(texR, env, rate);
    }

    return env;
}

