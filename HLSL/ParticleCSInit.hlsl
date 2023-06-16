#include "Particle.hlsli"

AppendStructuredBuffer<particle> particle_buffer : register(u1);
AppendStructuredBuffer<uint> count_buffer : register(u4);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{   
    //描画、更新用バッファとカウントバッファを登録
    uint id = dtid.x;
    particle p = ParticleInit();
    
    particle_buffer.Append(p);
    count_buffer.Append(id);
}