struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 ShadowParam : TEXCOORD1;
};

cbuffer CbScene : register(b0)
{
    float4 viewPosition;
    row_major float4x4 viewProjection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
    float4 ka;
    float4 kd;
    float4 ks;
    float shiness;
    float3 subset_dummy;
};

// シャドウマップ用コンスタントバッファ
cbuffer CBShadowMap : register(b10)
{
    row_major float4x4 ShadowVP; // 光の位置カメラ*投映
}
