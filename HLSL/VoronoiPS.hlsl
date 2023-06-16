#include "Voronoi.hlsli"

//Voronoi
Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

struct PS_OUT
{
    float4 Color : SV_TARGET0; // カラー
    float4 Depth : SV_TARGET1; // 距離
    float4 Normal : SV_TARGET2; // 法線
    float4 Position : SV_TARGET3; // 座標
    float4 RM : SV_TARGET4; // R+M
};

PS_OUT main(VS_OUT pin) : SV_TARGET
{
    PS_OUT output;
    
    float4 large = float4(0.1, 0.1, 0.05, 0.0);
    large = lenges;
    
     //何個となりのピクセルを見るか (元 0.0015)
    float add = 0.0015 * 1.25f;
    float2 nnn = SetVoro(pin.texcoord, large);
    
    //追加法線
    float3 NN = float3(0.5, 0.5, 1);
    
    float caX = nnn.x;
    float caY = nnn.y;
    
    //float4 diffuse_color = float4(1, 0, 0.25, 1);
    float4 diffuse_color = float4(0, 0.75, 0.75, 1);
    
    nnn = SetVoro(pin.texcoord + float2(add, 0), large);
    float caX1 = nnn.x;
    float caY1 = nnn.y;
    
    nnn = SetVoro(pin.texcoord + float2(-add, 0), large);
    float caX2 = nnn.x;
    float caY2 = nnn.y;
    
    nnn = SetVoro(pin.texcoord + float2(0, add), large);
    float caX3 = nnn.x;
    float caY3 = nnn.y;
    
    nnn = SetVoro(pin.texcoord + float2(0, -add), large);
    float caX4 = nnn.x;
    float caY4 = nnn.y;
    
    if (sqrt((caX - caX1) * (caX - caX1)) > 0.00001 ||
         sqrt((caY - caY1) * (caY - caY1)) > 0.00001)
    {
        NN.r = 1.0;
        NN.r = 0.0;
    }
    else if (sqrt((caX - caX2) * (caX - caX2)) > 0.00001 ||
         sqrt((caY - caY2) * (caY - caY2)) > 0.00001)
    {
        NN.r = 0.0;
        NN.r = 1.0;
    }
    
    if (sqrt((caX - caX3) * (caX - caX3)) > 0.00001 ||
         sqrt((caY - caY3) * (caY - caY3)) > 0.00001)
    {
        NN.g = 1.0;
        NN.g = 0.0;
    }
    else if (sqrt((caX - caX4) * (caX - caX4)) > 0.00001 ||
         sqrt((caY - caY4) * (caY - caY4)) > 0.00001)
    {
        NN.g = 0.0;
        NN.g = 1.0;
    }
    
    if (sqrt((caX - caX1) * (caX - caX1)) > 0.00001 ||
         sqrt((caY - caY1) * (caY - caY1)) > 0.00001 ||
         sqrt((caX - caX2) * (caX - caX2)) > 0.00001 ||
         sqrt((caY - caY2) * (caY - caY2)) > 0.00001 ||
         sqrt((caX - caX3) * (caX - caX3)) > 0.00001 ||
         sqrt((caY - caY3) * (caY - caY3)) > 0.00001 ||
         sqrt((caX - caX4) * (caX - caX4)) > 0.00001 ||
         sqrt((caY - caY4) * (caY - caY4)) > 0.00001
        )
    {
        //diffuse_color = float4(1, 0.25, 0.5, 1);
        
         //時間
        float time = lenges.w;
        //描画範囲
        float deowR = 0.15;
        
        float dorwNum = pin.texcoord.y;
        
        dorwNum = (pin.texcoord.x * 0.5f) + ((pin.texcoord.y) * 0.5f);
        
        {
            float draw_time = 1.0f / timeLine.x;
            time = frac(time * draw_time);
            
            float cole = timeLine.y / 2.0;
            time = lerp(-cole, 1.0 + cole, time);
            
            if (dorwNum < time + deowR && dorwNum > time - deowR)
            {
                diffuse_color = float4(1, 1, 1, 1);
                
                //補完を使った自然な白塗り
                float nn = dorwNum - time;
                nn = 1.0f - (sqrt(nn * nn) / deowR);
                diffuse_color = lerp(float4(1, 0, 0.25, 1), float4(1, 1, 1, 1), nn);
                diffuse_color = lerp(float4(0, 0.75, 0.75, 1),float4(1, 1, 1, 1), nn);
                //diffuse_color = lerp(diffuse_color, float4(1, 1, 1, 1), nn);
            }
        }
    }
    
    //return float4(NN, 1);
    
    float3 len = pin.world_position.xyz - camera_position.xyz;
    float leng = length(len);
    
    leng = saturate(leng / 50);
    
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(-lightDirection.xyz);
    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
    float3 N = NN;
    //	ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;
    diffuse_color *= lerp(power, 1, timeLine.z);
    diffuse_color.a = 1;
    
    output.Color = diffuse_color;
    float dist = length(pin.world_position.xyz - camera_position.xyz);
    output.Depth = float4(dist, pin.depthInView, dist, 1);
    output.Normal = float4(pin.normal * 0.5 + 0.5, 1);
    output.Normal = float4(N * 0.5 + 0.5, 1);
    output.Position = float4(pin.world_position, 1);
    float roughness = 1;
    float metallic = 1;
    output.RM = float4(roughness, metallic, 0, 1);
    return output;
}