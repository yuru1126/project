struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// シーンテクスチャ
Texture2D sceneTexture : register(t0);

Texture2D LightTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D RMTexture : register(t3);
Texture2D NormalTexture : register(t4);

// シーンサンプラステート
SamplerState sceneSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	// カラーの取得
    float4 tex = sceneTexture.Sample(sceneSampler, pin.texcoord);
    float4 light = LightTexture.Sample(sceneSampler, pin.texcoord);

    float4 N = NormalTexture.Sample(sceneSampler, pin.texcoord);
    float dist = length(N.xyz * 2.0 - 1.0);
    float n = step(0.5, dist);

    float4 color = tex * lerp(1.0, light, n);
    return color;
}