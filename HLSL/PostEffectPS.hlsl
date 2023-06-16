struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CBPostEffect : register(b0)
{
    float4 screenSize; // xy : �T�C�Y, z : near�N���b�v, w : far�N���b�v
	//	�J�������
    float4 viewPosition;
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 inverseView;
    row_major float4x4 inverseProjection;
	//	�F���␳
    float hueShift; // �F������
    float saturation; // �ʓx����
    float brightness; // ���x����
    float colorGradiationDummy;
};


// �V�[���e�N�X�`��
Texture2D sceneTexture : register(t0);

// Bloom
Texture2D BloomTexture[4] : register(t1);

// DOF
Texture2D BokehTexture : register(t5);
Texture2D DepthTexture : register(t6);

// ���Bloom����
Texture2D NormalTexture : register(t7);


// �V�[���T���v���X�e�[�g
SamplerState sceneSampler[3] : register(s0);

// �J���[�O���[�f�B���O
float3 ColorGrading(float3 color)
{
    float contrast = 1.3; //�R���g���X�g
    float saturation = 0.7; // �ʓx
    float3 colorlv = float3(0.9, 1.0, 1.1);

	// �R���g���X�g(���Â̍�)
    color = (color - 0.5) * contrast + 0.5;
	// �ʓx
    float gray = (color.r + color.g + color.b) / 3.0;
    color = (color - gray) * saturation + gray;

	// �F����
    color *= colorlv.rgb;
    return color;
}

// �r�l�b�g
float3 Vignette(float3 color, float2 position)
{
    float vignette = 1.0;
	// ���S����̋���
    float rate = length(position - 0.5);
    rate *= 1.5; // �S�̒���
    rate += (vignette - 1) * 0.5 - 0.1; // ���͈͒���
    rate = pow(rate, 3.0); // �������̕�����
    color = lerp(color, float3(0, 0, 0), saturate(rate));
    return color;
}


float4 main(VS_OUT pin) : SV_TARGET
{
	// �J���[�̎擾
    float4 tex = sceneTexture.Sample(sceneSampler[0], pin.texcoord);
    float4 color = tex;

	//1012 DOF
    float3 bokeh = BokehTexture.Sample(sceneSampler[0], pin.texcoord).rgb;
    float depth = DepthTexture.Sample(sceneSampler[0], pin.texcoord).r;
    float dist = 50.0;
    float focus = 50.0;
    float alpha = abs(depth - dist) / focus;
    alpha = saturate(alpha);
	
	// �@���̒���
    float4 normal = NormalTexture.Sample(sceneSampler[0], pin.texcoord);
    normal.xyz = normal.xyz * 2.0 - 1.0;
    float normal_length = length(normal.xyz);
    float bloomrate = normal_length * 0.7 + 0.3;
    
	// Bloom
    {
        uint mip_level = 0, width, height, number_of_levels;
        float4 bloom = BloomTexture[0].Sample(sceneSampler[2], pin.texcoord);
        float alpha = color.a;
        
        float3 blur_color = 0;
        float gaussian_kernel_total = 0;
        const int gaussian_half_kernel_size = 5;
        const float gaussian_sigma = 1.0;
        
        [unroll]
        for (int i = 1; i < 4; i++)
        {
            BloomTexture[i].GetDimensions(mip_level, width, height, number_of_levels);
            [unroll]
            for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
            {
                [unroll]
                for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
                {
                    float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) /
                    (2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);
                    
                    blur_color += BloomTexture[i].Sample(sceneSampler[1], pin.texcoord +
                    float2(x * 1.0f / width, y * 1.0 / height)).rgb * gaussian_kernel;
                    
                    gaussian_kernel_total += gaussian_kernel;
                }
            }
        }
        blur_color /= gaussian_kernel_total;
        const float bloom_intensity = 1.0f;
        color.rgb += blur_color * bloom_intensity;
    }
    
	// �J���[�O���[�f�B���O
    color.rgb = ColorGrading(color.rgb);
    
    return color;
}