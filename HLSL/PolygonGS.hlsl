#include "Polygon.hlsli"
#include "Mathf.hlsli"

#define PI 3.141259

[maxvertexcount(4)]
void main(triangle DS_OUT input[3] : SV_POSITION, uint pid : SV_PrimitiveID,
inout TriangleStream<GS_OUT> output)
{
    //3点から中心点、法線を求める
    float3 wp0 = input[0].position.xyz;
    float3 wp1 = input[1].position.xyz;
    float3 wp2 = input[2].position.xyz;
    float3 center = CenterPoint(wp0, wp1, wp2); //3点の中心点
    float3 normal = ConstructNormal(wp0, wp1, wp2); //3点からなる法線
    
    //それぞれの中心点を使いランダム値を生成
    float move_pos_rand = (random(float2((pid + 10) * 3.22546 * center.x, (pid + 1) * 8.97525 * center.y)) - 0.5) * 2;
    float Yup_rand = (random(float2((pid + 10) * 3.22546 * center.x, (pid + 1) * 8.97525 * center.y)) - 0.5);
    float draw_judge_rand = random(float2((pid + 10) * 7.72546 * center.x, (pid + 1) * 2.46525 * center.y)) * 0.5f + 0.5;
    float length_rand = random(float2((pid + 10) * 4.32546 * center.x, (pid + 1) * 5.97525 * center.y));
    float color_rand = random(float2((pid + 5) * 9.13546 * center.x, (pid + 7) * 7.83525 * center.y)) * 0.5f + 0.5;
    
    float destruction = destruction_factor;
    
    
    //死亡演出
    if (geometry_type == DEATH || geometry_type == DEATH_NOT_TIMER)
    {
        {
            float delay_time = 1.0f; //補間が始まる時間
            float move_time = 1.5f; //補間が終わる時間
            move_time = 1 / move_time;
            
            destruction = pow(saturate((time - delay_time) * move_time), 2);
        }
        
        [unroll]
        for (uint i = 0; i < 3; i++)
        {
            GS_OUT element;
        
            float3 pos = input[i].position;
        
            //サイズ変更
            float size = lerp(3.5, 1.0, destruction);
            pos = (pos - center) * pow((1.0 - destruction * scale_factor), size) + center;
        
            //法線変更
            pos = rotate(pos - center, move_pos_rand * destruction * rotation_factor) + center;
        
            //位置変更
            pos.xz += normal.xz * destruction * position_factor * ((move_pos_rand / 2) + 0.5);
            pos.y += max(0, normal.y) * destruction * position_factor * ((move_pos_rand / 2) + 0.5);
        
            //Y方向変更
            pos.y += destruction * yplas_factor * 5 * ((1.0 - ((move_pos_rand / 2) + 0.5)) * 0.5 + 0.5);
        
            element.position = mul(float4(pos, 1.0), viewProjection);
            element.world_position = pos;
            element.color = input[i].color;
            element.normal = input[i].normal;
            element.texcoord = input[i].texcoord;
            element.depthInView = element.position.z;
            element.type = DEATH;
            output.Append(element);
        
            input[i].position = float4(pos, 1.0);
        }
    }
    else if (geometry_type == BIRTH || geometry_type == BIRTH_NOT_TIMER)
    {
        //登場演出
        
        //Destruction   0.0  ～ 1.0
        //              1.15 ～ -0.5
        //              1.65 ～ -0.0
        float Yup = lerp(interpolation_vertical.x, interpolation_vertical.y, destruction);
        
        if (geometry_type == BIRTH)
        {
            destruction = min(1, (time - 3) * 0.4);
            
            float delay_time = 0.0f; //補間が始まる時間
            float move_time = 2.75f; //補間が終わる時間
            move_time = 1 / move_time;
            
            Yup = lerp(interpolation_vertical.x, interpolation_vertical.y,
            (max(0, (time - delay_time) * move_time)));
        }
    
        //Y方向を判定し、上昇していなければそのまま描画する
        if (Yup < (vertical_end + model_position.y - center.y))
        {
            [unroll]
            for (uint i = 0; i < 3; i++)
            {
                GS_OUT element;
                float3 pos = input[i].position;
                element.position = mul(float4(pos, 1.0), viewProjection);
                element.world_position = pos;
                element.color = input[i].color;
                element.normal = input[i].normal;
                element.texcoord = input[i].texcoord;
                element.depthInView = element.position.z;
            
                element.type = BIRTH_AFTER;
            
                output.Append(element);
            }
        }
        else
        {
            //3点の中心点と面積から正方形を生成
            
            //正方形情報
            const float3 billboard[] =
            {
                float3(-1.0f, -1.0f, 0.0f),
		        float3(-1.0f, +1.0f, 0.0f),
		        float3(+1.0f, -1.0f, 0.0f),
		        float3(+1.0f, +1.0f, 0.0f),
            };
            const float2 texcoords[] =
            {
                float2(0.0f, 1.0f),
		        float2(0.0f, 0.0f),
		        float2(1.0f, 1.0f),
		        float2(1.0f, 0.0f), 
            };
            
            //Yを上昇させる
            float up = -Yup + (vertical_end + model_position.y - center.y);
            up -= Yup_rand * 0.05f;
            up *= -1;
            
            //xzの位置から大きさをとり移動
            float2 xz = center.xz - model_position.xz;
            float2 vec_normal = normalize(xz);
            float len = length(xz) + 0.15 * length_rand;
            center.xz = model_position.xz + vec_normal * len;
            
            //中心点とYの上昇値から正方形の中心点を設定
            float3 pos = center + float3(0, up, 0);
            
            //三角形の面積からおおよその半径を取り正方形の大きさを指定
            float3 ab = wp1 - wp0;
            float3 ac = wp2 - wp0;
            float area = length(cross(ab, ac));
            area = sqrt(area) * 0.5;
            area *= 1.5f;
        
            //色をランダムに変更
            float3 bigin_color = float3(74, 221, 216) / 256;
            float3 end_color = float3(88, 74, 221) / 256;
            float4 square_color = float4(lerp(bigin_color, end_color, color_rand), 1);
            
            //描画数を制限
            float fraction = 20;
            square_color.a = step(draw_judge_rand, 1 / fraction);
            
            //Yの高さに応じてサイズを調整
            float area_size = Pay(area_size_vertical.x, area_size_vertical.y, pos.y - model_position.y);
            
            //法線をカメラ方向に向ける
            normal = normalize(pos - camera_position.xyz);
            float3 Z = normal;
            float3 X = normalize(cross(Z, float3(0, 1, 0)));
            float3 Y = normalize(cross(Z, X));
            row_major float3x3 R = { X, Y, Z };
            
            [unroll]
            for (uint ii = 0; ii < 4; ii++)
            {
                GS_OUT element;
                float3 corner_pos = billboard[ii] * area * area_size;
            
                element.world_position = pos + mul(corner_pos, R);
                element.position = mul(float4(pos + mul(corner_pos, R), 1), viewProjection);
                element.color = square_color;
                
                element.normal = normal;
                element.texcoord = texcoords[ii];
                element.depthInView = element.position.z;
                element.type = BIRTH;
                output.Append(element);
            }
        }
    }
    output.RestartStrip();
    return;
}