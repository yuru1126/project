//死亡演出
#define DEATH 0

//出現関連
#define BIRTH 1
#define BIRTH_AFTER 2

//タイマー感知
#define DEATH_NOT_TIMER 3
#define BIRTH_NOT_TIMER 4

struct VS_OUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

//ハル（分割数指定）
struct HSConstantOutput
{
    float factor[3] : SV_TessFactor;
    float inner_factor : SV_InsideTessFactor;
};

struct HS_OUT
{
    float2 texcoord : TEXCOORD;
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct DS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 world_position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float3 world_position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float depthInView : DEPTH;
    int type : TYPE;
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

cbuffer CbPolygon : register(b3)
{
    float time;
    float destruction_factor;   //補間変数
    float position_factor;      //移動値
    float rotation_factor;      //回転値
    float scale_factor;         //サイズ縮小値
    float yplas_factor;         //Y上昇値
    
    float multiply;             //ポリゴン分割数調整(値が低いとポリゴンの大きさが統一されない)
    int   geometry_type;
    
    //Y方向の補間値
    float2 interpolation_vertical;
    //Yの位置から面積の大きさを変更する
    float2 area_size_vertical;
    //モデルのYの最大値の少し上
    float vertical_end;
    //モデルのポジション
    float3 model_position;
};

//3点からなる三角形の法線を返す
float3 ConstructNormal(float3 v1, float3 v2, float3 v3)
{
    return normalize(cross(v2 - v1, v3 - v1));
}

//3点の中心点を返す
float3 CenterPoint(float3 v1, float3 v2, float3 v3)
{
    return (v1 + v2 + v3) / 3.0;
}

float3 rotate(float3 p, float3 rotation)
{
    //rotationがゼロ行列だと、Geometry shaderが表示されないので注意
    float3 a = normalize(rotation);
    float angle = length(rotation);
    
    //rotationがゼロ行列のときの対応
    if (abs(angle) < 0.001) return p;
    
    float s = sin(angle);
    float c = cos(angle);
    float r = 1.0 - c;
    float3x3 m = float3x3(
                a.x * a.x * r + c,
                a.y * a.x * r + a.z * s,
                a.z * a.x * r - a.y * s,
                a.x * a.y * r - a.z * s,
                a.y * a.y * r + c,
                a.z * a.y * r + a.x * s,
                a.x * a.z * r + a.y * s,
                a.y * a.z * r - a.x * s,
                a.z * a.z * r + c
            );

    return mul(m, p);
}