#include "Particle.hlsli"
#include "FluidSimulation.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
AppendStructuredBuffer<uint> count_buffer : register(u4);

//流体シミュレーション用
RWStructuredBuffer<Velocity> X_buffer : register(u5);
RWStructuredBuffer<Velocity> Y_buffer : register(u6);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    if (id <= particle_count)
    {
        particle p = particle_buffer[id];
        
        //距離によるサイズ縮小
        p.length_shrink.x = saturate(length(p.position.xyz - camera_position.xyz) / divide_length);
        p.length_shrink.x = max(p.length_shrink.x, (1 - p.length_shrink.y));
                
        //方向をカメラの向きに向ける
        p.normal.xyz = normalize(p.position.xyz - camera_position.xyz);
        
        //位置から影響を受ける範囲を特定
        float x = Pay(-7.5, 2.5, p.position.x);
        float y = Pay(-5.0, 5.0, p.position.y);
                
        //3角形要素内の補間を行うためx,yの近似点とその距離を求める
        float floatX = x * 11.0;
        float floatY = y * 11.0;
        int nearX = round(floatX);
        int nearY = round(floatY);
        float subX = floatX - nearX;
        float subY = floatY - nearY;
        
        int nodeX = -1;
        int nodeY = -1;
        if (subX > 0) nodeX = 1;
        if (subY > 0) nodeY = 1;
                
        float absX = abs(subX);
        float absY = abs(subY);
        
        float vecX = (X_buffer[XNArray(nearX, nearY)].velocity +
                (X_buffer[XNArray((nearX + nodeX) % 12, nearY)].velocity - X_buffer[XNArray(nearX, nearY)].velocity) * absX +
                (X_buffer[XNArray(nearX, (nearY + nodeY) % 12)].velocity - X_buffer[XNArray(nearX, nearY)].velocity) * absY);
                
        float vecY = (Y_buffer[YNArray(nearX, nearY)].velocity +
                (Y_buffer[YNArray((nearX + nodeX) % 12, nearY)].velocity - Y_buffer[YNArray(nearX, nearY)].velocity) * absX +
                (Y_buffer[YNArray(nearX, (nearY + nodeY) % 12)].velocity - Y_buffer[YNArray(nearX, nearY)].velocity) * absY);
        
        p.position.x += vecX * elapsed_time;
        p.position.y += vecY * elapsed_time;
        
        particle_buffer[id] = p;
    }
}