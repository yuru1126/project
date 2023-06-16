#include "Particle.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
RWStructuredBuffer<particle> particle_register_buffer : register(u2);
ConsumeStructuredBuffer<uint> count_buffer : register(u5);

[numthreads(128, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    //セットしたパーティクルを描画、更新用バッファに登録する
    uint id = dtid.x;
    //現在使われていないidを取ってくる
    uint not_use_id = count_buffer.Consume();
    //1つずつ設定した分だけ配列をずらす
    particle_buffer[not_use_id] = particle_register_buffer[id + count];
}