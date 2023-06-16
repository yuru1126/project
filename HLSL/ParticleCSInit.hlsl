#include "Particle.hlsli"

AppendStructuredBuffer<particle> particle_buffer : register(u1);
AppendStructuredBuffer<uint> count_buffer : register(u4);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{   
    //�`��A�X�V�p�o�b�t�@�ƃJ�E���g�o�b�t�@��o�^
    uint id = dtid.x;
    particle p = ParticleInit();
    
    particle_buffer.Append(p);
    count_buffer.Append(id);
}