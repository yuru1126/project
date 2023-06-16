struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 world_position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
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

cbuffer CbVoronoi : register(b3)
{
    float4 lenges;
    float4 timeLine;
};


float SetLenge(float2 a, float2 b)
{
    float x = a.x - b.x;
    float y = a.y - b.y;
    return sqrt(x * x + y * y);
}

float SetLenge(float a, float b)
{
    float x = a - b;
    return sqrt(x * x);
}

//ボロノイ返却
float2 SetVoro(float2 uv, float4 la)
{
    float HangX = 10.0f;
    float HangY = 10.0f;
    if (la.x > 0.001)
        HangX = 1.0f / la.x;
    if (la.y > 0.001)
        HangY = 1.0f / la.y;
    float nx = uv.x * HangX;
    float ny = uv.y * HangY;
    int countY = 0;
    countY = floor(ny);
    
    nx = floor(nx) / HangX;
    ny = floor(ny) / HangY;
    float2 mypos = float2(uv.x, uv.y);
    float lenge2 = 2000.0;
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 vp = float2(nx + la.x * x, ny + la.y * y);
           
            int co = countY;
            co += y;
            if (co % 2 == 0)
            {
                vp.x += la.z;
            }
          
            float len = (SetLenge(mypos, vp));
            if (lenge2 > len)
            {
                uv = vp; //テクスチャボロノイ
            }
            lenge2 = min(lenge2, len);
        }
    }
    
    return uv;
}