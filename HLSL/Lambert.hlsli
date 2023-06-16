struct VS_OUT
{
	float4 position : SV_POSITION;
    float3 world_position : POSITION;
    float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    float depthInView : DEPTH;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 camera_position;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
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