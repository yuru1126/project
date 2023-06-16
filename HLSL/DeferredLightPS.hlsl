struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// ライト情報
cbuffer CBLight : register(b1)
{
	//w=0:xyz=方向 w>0:xyz位置 w減衰距離
    float4 LightDirection;
    float4 LightColor; //色
};

// シーンテクスチャ
Texture2D sceneTexture : register(t0);

Texture2D LightTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D RMTexture : register(t3);
Texture2D NormalTexture : register(t4);

// 環境テクスチャ
Texture2D EnvTexture : register(t15);


// シーンサンプラステート
SamplerState sceneSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	// カラーの取得
    float4 tex = sceneTexture.Sample(sceneSampler, pin.texcoord);
	// 点光源方向
    float4 P = PositionTexture.Sample(sceneSampler, pin.texcoord);
    float3 PLightPos = LightDirection.xyz;
    float3 PLightDir = P.xyz - PLightPos;
	// ライト種別
    float type = step(0.001, LightDirection.w);
	// 減衰の設定
    float attenuation = type < 0.1 ? 1.0 : 1.0 - (length(PLightDir) / LightDirection.w);
    attenuation = saturate(attenuation);
	// 平行光
    float3 L = lerp(LightDirection.xyz, PLightDir, type);

    float3 N = tex.xyz * 2.0 - 1.0;
    float3 C = LightColor.rgb * 2;
    C *= attenuation; //減衰

    L = normalize(L);
    N = normalize(N);
    float d = -dot(N, L);
    d = saturate(d); // 0～1にクリッピング
    tex.rgb = C * d; //ライト決定

    return tex;
}

