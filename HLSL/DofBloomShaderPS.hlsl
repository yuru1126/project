struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
Texture2D DiffuseTexture : register(t0);
Texture2D DepthTexture : register(t1);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
// ガウスフィルタ
//--------------------------------------------
static const float waight[] =
{
    { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 },
    { 2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0 },
    { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 },
};

float4 main(VS_OUT input) : SV_TARGET0
{
    float4 color = DiffuseTexture.Sample(DecalSampler,
			input.texcoord) * input.color;
	//テクスチャサイズ取得
    uint width, height;
    DiffuseTexture.GetDimensions(width, height);
	//テクセルサイズ取得
    const float2 texcel = float2(2 * 2 / (float) width, 2 * 2 / (float) height);
    float3 work = (float3) 0;
    float total = 0;
    for (int y = -3; y <= 3; y++)
    {
        for (int x = -3; x <= 3; x++)
        {
            float weight = (x * x + y * y) / 10.0;
            weight = saturate(1.0 - weight);
			// ピント計算
            float depth = DepthTexture.Sample(DecalSampler,
							input.texcoord + float2(texcel.x * x, texcel.y * y)).r;
            float dist = 50.0;
            float focus = 50.0;
            float alpha = abs(depth - dist) / focus;
            alpha = saturate(alpha);
            weight *= alpha;

            work += DiffuseTexture.Sample(DecalSampler, input.texcoord + float2(texcel.x * x, texcel.y * y)).rgb
						* weight;
            total += weight;
        }
    }
    color.rgb = work / total;
    return color;
}
