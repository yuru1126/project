#include "Particle.hlsli"

AppendStructuredBuffer<particle> particle_buffer : register(u3);

[numthreads(128, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    //登録用バッファにパーティクルを登録
    uint id = dtid.x;
    particle p = SetParticle(particle_data, id * elapsed_time * 8.3461 + Progress_time, Progress_time, sub_emitter_flag);
    particle_buffer.Append(p);
}