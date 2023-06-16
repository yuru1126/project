#include "Mathf.hlsli"

#define FLT_MIN          1.175494351e-38F


cbuffer PARTICLE_CONSTANTS : register(b0)
{
    uint particle_count;
    float particle_size;
    float elapsed_time;
    float Progress_time;
    
    float4 setHSV;
    
    float divide_length;
    int count;
    
    
    float2 dummy;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 camera_position;
};

//パーティクルを設定するデータ
struct ParticleData
{
    float4 position_max;
    float4 position_min;
    float4 velocity_max;
    float4 velocity_min;
    float4 color_max;
    float4 color_min;
    
    float2 lifespan; //寿命
    float set_divide_age;
    
    float gravity; //重力影響度
    float draw_size; //描画サイズ
    
    //タイルのアニメーション再生速度
    float animation_speed;
    
    //フラグ
    float length_shrink_flag; //距離によるサイズ縮小
    float appearance_frequency; //サブエミッターの出現頻度
    float4 dummyF1;
    
    int draw_type; //描画タイプ
    //描画タイル設定
    int3 tiles;
    int2 set_tiles; //x:max y:min
    int set_state; //
    
    //フラグ関連
    int hsv_data_flag; //HSVデータかの判定
    int velocity_normalize_flag; //速度の正規化
    
    int3 dummyI1;
};

//パーティクル
struct particle
{
    float4 position;
    float4 velocity;
    float4 normal;
    float4 color;
    float gravity;
    float age;
    
    float2 length_shrink;
    float size;
    float divide_age;
    
    float use_tile;
    float animation_speed;
    
    float appearance_timer;
    float appearance_frequency;
    
    float2 dummyF;
    
    
    int draw_type;
    int3 tiles;
    int state;
    int sub_emitter_flag;
    int2 dummyI;
    
    //サブエミッターの設定をパーティクルに持たせるには大きすぎたため定数バッファの値を参照する
};


cbuffer CbParticle : register(b2)
{
    ParticleData particle_data;
    ParticleData death_emitter_data;
    ParticleData survival_emitter_data;
    
    int sub_emitter_flag; //サブエミッター設定
    float3 dummy2;
};

struct VS_OUT
{
    uint vertex_id : VERTEXID;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    int draw_type : DRAW_TYPE;
};

//パーティクルデータ初期化
ParticleData ParticleDataInit()
{
    ParticleData data;
    
    data.position_max = 0;
    data.position_min = 0;
    
    data.velocity_max = 0;
    data.velocity_min = 0;
    data.color_max = 0;
    data.color_min = 0;
    data.lifespan = 0;
    data.set_divide_age = 0;
    data.gravity = 0;
    data.draw_size = 0;
    data.animation_speed = 0;
    data.length_shrink_flag = 0;
    data.appearance_frequency = 0;
    data.dummyF1 = 0;
    
    data.draw_type = 0;
    data.tiles = 0;
    data.set_tiles = 0;
    data.set_state = 0;
    data.hsv_data_flag = 0;
    data.velocity_normalize_flag = 0;
    data.dummyI1 = 0;
    
    return data;
};

//パーティクル初期化
particle ParticleInit()
{
    particle p;
    
    p.position = 0;
    p.velocity = 0;
    p.normal = 0;
    p.color = 0;
    p.gravity = 0;
    p.age = 0;
    p.length_shrink = 0;
    p.size = 0;
    p.divide_age = 0;
    p.use_tile = 0;
    p.animation_speed = 0;
    
    p.appearance_timer = 0;
    p.appearance_frequency = 0;
    
    p.draw_type = 0;
    p.tiles = 0;
    p.state = 0;
    p.sub_emitter_flag = 0;
    
    p.dummyI = 0;
    p.dummyF = 0;
    
    return p;
};


//パーティクルデータを元にパーティクルを設定
particle SetParticle(ParticleData data, float random_factor, float random_color)
{
    particle p = ParticleInit();
    float4 vec_ran;
    vec_ran.x = random(float2(random_factor + 1, random_factor + 2)) * 0.5 + 0.5;
    vec_ran.y = random(float2(random_factor + 3, random_factor + 4)) * 0.5 + 0.5;
    vec_ran.z = random(float2(random_factor + 5, random_factor + 6)) * 0.5 + 0.5;
    vec_ran.w = random(float2(random_factor + 9, random_factor + 10)) * 0.5 + 0.5;
        
    float4 color_ran;
    color_ran.x = random(float2(random_color + 11, random_color + 12)) * 0.5 + 0.5;
    color_ran.y = random(float2(random_color + 13, random_color + 14)) * 0.5 + 0.5;
    color_ran.z = random(float2(random_color + 15, random_color + 16)) * 0.5 + 0.5;
    color_ran.w = random(float2(random_color + 17, random_color + 18)) * 0.5 + 0.5;
    
    float age_ran = random(float2(random_factor + 18, random_factor + 19)) * 0.5 + 0.5;
    float4 pos_ran;
    pos_ran.x = random(float2(random_factor + 20, random_factor + 21)) * 0.5 + 0.5;
    pos_ran.y = random(float2(random_factor + 22, random_factor + 23)) * 0.5 + 0.5;
    pos_ran.z = random(float2(random_factor + 24, random_factor + 25)) * 0.5 + 0.5;
    pos_ran.w = random(float2(random_factor + 28, random_factor + 27)) * 0.5 + 0.5;
    
    
    p.position.xyz = lerp(data.position_min.xyz, data.position_max.xyz, pos_ran.xyz);
    
    p.velocity.xyz = lerp(data.velocity_min.xyz, data.velocity_max.xyz, vec_ran.xyz);
    
    if (length(p.velocity.xyz) > 0.0)
    {
        float vec = saturate(data.velocity_normalize_flag);
        float vec_length = lerp(data.velocity_min.w, data.velocity_max.w, vec_ran.w);
        float3 vec_normal = normalize(p.velocity.xyz) * vec_length;
        p.velocity.xyz = lerp(p.velocity.xyz, vec_normal, vec);
    }
    
    p.velocity.w = 0;
        
    p.normal = float4(0, 0, 1, 0);
        
    p.color = lerp(data.color_min, data.color_max, color_ran);
        
    if (data.hsv_data_flag != 0)
    {
        p.color.rgb = HSV2RGB(p.color.rgb);
    }
    p.color.w = 1;
        
        
    p.age = lerp(data.lifespan.x, data.lifespan.y, age_ran);
    p.state = data.set_state;
    p.length_shrink = data.length_shrink_flag;
    p.draw_type = data.draw_type;
    p.gravity = data.gravity;
    p.size = data.draw_size;
    p.divide_age = max(0.001, data.set_divide_age);
        
    float tile_ran = random(float2(random_factor + 7, random_factor + 8)) * 0.5 + 0.5;
    p.tiles = data.tiles;
    int sub = data.set_tiles.y - data.set_tiles.x;
    float pla = round(lerp(0, sub, tile_ran));
    p.use_tile = data.set_tiles.x + pla;
    p.animation_speed = data.animation_speed;
    
    p.appearance_timer = data.appearance_frequency;
    p.appearance_frequency = data.appearance_frequency;
    
    //p.dummyF = 0;
    p.dummyI = 0;
    
    return p;
}

particle SetParticle(ParticleData data, float random_factor, float random_color, int sub_flag)
{
    particle p = SetParticle(data, random_factor, random_color);
    p.sub_emitter_flag = sub_flag;
    return p;
}