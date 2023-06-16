#include "Particle.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
RWStructuredBuffer<particle> particle_register_buffer : register(u2);
ConsumeStructuredBuffer<uint> count_buffer : register(u5);

[numthreads(128, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    //�Z�b�g�����p�[�e�B�N����`��A�X�V�p�o�b�t�@�ɓo�^����
    uint id = dtid.x;
    //���ݎg���Ă��Ȃ�id������Ă���
    uint not_use_id = count_buffer.Consume();
    //1���ݒ肵���������z������炷
    particle_buffer[not_use_id] = particle_register_buffer[id + count];
}