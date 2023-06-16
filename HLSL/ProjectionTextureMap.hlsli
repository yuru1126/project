struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};
cbuffer CbProjection : register(b0)
{
    row_major float4x4 InverseViewProjection;
    row_major float4x4 projectionTextureViewProjection;
}

cbuffer CbCircle : register(b1)
{
    row_major float4x4 CircleTransform;
    int CircleFlag;
    float3 subset_dummy;
}