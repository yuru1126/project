// HDRI(パノラマ画像)の映り込み
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


//	テクスチャ
Texture2D MainTexture : register(t0);
Texture2D LightTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D RMTexture : register(t3);
// SSR
Texture2D DepthTexture : register(t4);
Texture2D ColorTexture : register(t5);


Texture2D EnvTexture : register(t15);

// シーンサンプラステート
SamplerState sceneSampler : register(s0);
SamplerState ClampSampler : register(s1);

// SSR
// 映りこむ画面テクスチャのUVを返す
// P : 座標　R:視線の反射
float2 SSR(float3 P, float3 R)
{
	// 反射ベクトル(視線と法線で反射)
    R = normalize(R);
	// ちょっとずつ進める
    R *= 0.3; //0.1 = 一回で進める距離
	[loop] //Loopのままで良いよ。
    for (int i = 0; i < 100; i++)
    {
        P += R; //進める
		// 何かにぶつかった？
		// Pまでの距離と
        float DistP = length(P - viewPosition.xyz);
		// レンダリング結果の距離で比較
		// Pのスクリーン座標 → UV → Depthから距離取得
        float4 ScreenP = mul(float4(P, 1.0), viewProjection);
		// スクリーン座標(-1 <--> 1) → UV(0<-->1)
        float2 Puv = (ScreenP.xy / ScreenP.w) * 0.5 + 0.5;
        Puv.y = 1.0 - Puv.y; //Y反転
		// 画面外判定
        if (Puv.x < 0) return -1;
        if (Puv.y < 0) return -1;
        if (Puv.x > 1) return -1;
        if (Puv.y > 1) return -1;
							 		 
		// Depthから距離取得
        float DistS = DepthTexture.Sample(
			sceneSampler, Puv).x;

		// 距離比較(２以上近い場合はNG)
        if (DistP > DistS && DistP - 2 < DistS)
        {
			// 交差 = 映り込みポイント
            return Puv;
        }
    }
    return -1; // 映り込み無
}

float4 main(VS_OUT pin) : SV_TARGET
{
	// 法線の取得
    float4 tex = MainTexture.Sample(sceneSampler, pin.texcoord);
    float3 orgN = tex.xyz * 2.0 - 1.0;
    float3 N = normalize(orgN);

	// 視線の反射
    float4 P = PositionTexture.Sample(sceneSampler, pin.texcoord);
    float3 E = P.xyz - viewPosition.xyz; //視線
    float3 R = reflect(normalize(E), N);
	// Rを上下と方向に分解
    float2 v;
    v.y = R.y; //Yはそのまま
    float3 work = R;
    work.y = 0; //高さ無視
    work = normalize(work); //方角のみ
    v.x = work.x;
	//ひずみを直す

    float2 uv = v;
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
    uv.x *= 0.5;
	// 南向き
    if (R.z > 0)
    {
        uv.x = 1.0 - uv.x;
    }
	//Roughnessを考えてみる
    float4 RM = RMTexture.Sample(sceneSampler, pin.texcoord);
    float roughness = RM.x;
	//roughness = 1;
	//r 0.0  0.3   0.5  1.0 
	//l 0.0  8.0  10.0  11.0
    float w = 1.0 - pow(1.0 - roughness, 5.0);
    float level = w * 12.0;
    float4 env = EnvTexture.SampleLevel(ClampSampler, uv, level);
	//単純なトーンマッピング
    env.rgb = env.rgb / (100 + env.rgb);

	// SSR 映り込みのUVを受け取る
    float2 ssr = SSR(P.xyz, R);
    if (ssr.x >= 0)
    {
		// Mipmap対応
        float level = roughness * 5.0;
        float4 texR = ColorTexture.SampleLevel(
					sceneSampler, ssr, level);
		// ブレンド
        float rate = roughness;
        rate = rate * 0.6 + 0.4; //0.4---1.0
        env = lerp(texR, env, rate);
    }

    return env;
}

