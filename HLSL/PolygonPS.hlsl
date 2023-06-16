#include "Polygon.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

struct PS_OUT
{
    float4 Color : SV_TARGET0; // �J���[
    float4 Depth : SV_TARGET1; // ����
    float4 Normal : SV_TARGET2; // �@��
    float4 Position : SV_TARGET3; // ���W
    float4 RM : SV_TARGET4; // R+M
};

PS_OUT main(GS_OUT pin) : SV_TARGET
{
    PS_OUT output;
    
    //���o�ɉ����ăJ���[��ύX
    if (pin.type == DEATH)
    {
        //���Ԏ��S���o
        //��Ԓ��ɂ��񂾂񔒂�����
        float delay_time = 1.0f; //��Ԃ��n�܂鎞��
        float move_time = 1.5f; //��Ԃ��I��鎞��
        move_time = 1 / move_time;
        float destruction = pow(saturate((time - delay_time) * move_time * 2), 2);
        float4 leColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
        pin.color = float4(0.9, 1, 0.9, 1);
        leColor.rgb = lerp(leColor.rgb, pin.color.rgb, destruction);
    
        output.Color = leColor;
    }
    else if (pin.type == BIRTH)
    {
        //�o�����o(�����`��)
        //�w�肵���J���[�����̂܂܎g�p
        output.Color = pin.color;
    }
    else if (pin.type == BIRTH_AFTER)
    {
        //�o�����o(�ʏ�`��)
        //�ʏ�̕`��ɖ߂��Ă����͐F�𔒂�����
        float4 leColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
        
        float delay_time = 0.0f; //��Ԃ��n�܂鎞��
        float move_time = 2.75f; //��Ԃ��I��鎞��
        move_time = 1 / move_time;
        
        float destruction = lerp(interpolation_vertical.x, interpolation_vertical.y,
        (max(0, (time - delay_time) * move_time)));
        destruction = saturate((vertical_end * 1.5 + model_position.y - pin.world_position.y) - destruction);
        
        pin.color = float4(0.9, 1, 0.9, 1);
        leColor.rgb = lerp(pin.color.rgb, leColor.rgb, destruction);
        output.Color = leColor;
    }
    
    float dist = length(pin.world_position.xyz - camera_position.xyz);
    output.Depth = float4(dist, pin.depthInView, dist, 1);
    output.Normal = float4(pin.normal * 0.5 + 0.5, 1);
    output.Position = float4(pin.world_position, 1);
    float roughness = 1;
    float metallic = 1;
    output.RM = float4(roughness, metallic, 0, 1);
    
    return output;
}