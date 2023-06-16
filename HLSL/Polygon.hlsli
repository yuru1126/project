//���S���o
#define DEATH 0

//�o���֘A
#define BIRTH 1
#define BIRTH_AFTER 2

//�^�C�}�[���m
#define DEATH_NOT_TIMER 3
#define BIRTH_NOT_TIMER 4

struct VS_OUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

//�n���i�������w��j
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
    float destruction_factor;   //��ԕϐ�
    float position_factor;      //�ړ��l
    float rotation_factor;      //��]�l
    float scale_factor;         //�T�C�Y�k���l
    float yplas_factor;         //Y�㏸�l
    
    float multiply;             //�|���S������������(�l���Ⴂ�ƃ|���S���̑傫�������ꂳ��Ȃ�)
    int   geometry_type;
    
    //Y�����̕�Ԓl
    float2 interpolation_vertical;
    //Y�̈ʒu����ʐς̑傫����ύX����
    float2 area_size_vertical;
    //���f����Y�̍ő�l�̏�����
    float vertical_end;
    //���f���̃|�W�V����
    float3 model_position;
};

//3�_����Ȃ�O�p�`�̖@����Ԃ�
float3 ConstructNormal(float3 v1, float3 v2, float3 v3)
{
    return normalize(cross(v2 - v1, v3 - v1));
}

//3�_�̒��S�_��Ԃ�
float3 CenterPoint(float3 v1, float3 v2, float3 v3)
{
    return (v1 + v2 + v3) / 3.0;
}

float3 rotate(float3 p, float3 rotation)
{
    //rotation���[���s�񂾂ƁAGeometry shader���\������Ȃ��̂Œ���
    float3 a = normalize(rotation);
    float angle = length(rotation);
    
    //rotation���[���s��̂Ƃ��̑Ή�
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