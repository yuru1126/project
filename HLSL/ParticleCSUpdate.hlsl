#include "Particle.hlsli"
#include "FluidSimulation.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);
AppendStructuredBuffer<uint> count_buffer : register(u4);

AppendStructuredBuffer<particle> append_particle_buffer : register(u3);

#define PI 3.141259

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{    
    uint id = dtid.x;
    
    if (id <= particle_count)
    {
        if (particle_buffer[id].age > 0)
        {
            particle p = particle_buffer[id];
            
            if (p.state == 0)
            {
                //��������
                p.age -= elapsed_time;
            
                //�����������Ȃ�Ε`���؂�ID��Ԃ�
                if (p.age <= 0)
                {
                    //�T�u�G�~�b�^�[�ݒ肪����Γo�^�o�b�t�@�ɓo�^
                    if (p.sub_emitter_flag == 1)
                    {
                        float rand_factor = p.velocity.x + p.velocity.y * 21.989 + p.velocity.z * 37.741 + Progress_time;
                    
                        [loop]
                        for (uint i = 0; i < 128; ++i)
                        {
                            particle set = SetParticle(death_emitter_data, rand_factor + i, rand_factor);
                            set.position = p.position;
                            append_particle_buffer.Append(set);
                        }
                    }
                    if (p.sub_emitter_flag == 3)
                    {
                        float rand_factor = p.velocity.x + p.velocity.y * 21.989 + p.velocity.z * 37.741 + Progress_time;
                    
                        [loop]
                        for (uint i = 0; i < 128; ++i)
                        {
                            particle set = SetParticle(death_emitter_data, rand_factor + i, rand_factor);
                            set.position = p.position;
                            set.sub_emitter_flag = 2;
                            append_particle_buffer.Append(set);
                        }
                    }
                    
                    p.color.a = 0;
                    p.length_shrink = 0;
                    //�g��Ȃ��Ȃ���id���J�E���g�o�b�t�@�ɕԂ�
                    count_buffer.Append(id);
                }
            
                //�����ɂ��T�C�Y�k��
                p.length_shrink.x = saturate(length(p.position.xyz - camera_position.xyz) / divide_length);
                p.length_shrink.x = max(p.length_shrink.x, (1 - p.length_shrink.y));
            
                //�����ɂ��T�C�Y�k��
                float multiply = saturate(p.age / p.divide_age);
                p.length_shrink.x *= multiply;
            
                //�������J�����̌����Ɍ�����
                p.normal.xyz = normalize(p.position.xyz - camera_position.xyz);
            
                //�ʒu�X�V
                p.position += p.velocity * elapsed_time;
            
                //�d�͉����x
                p.velocity.y -= 9.8 * elapsed_time * p.gravity;
                
                //�A�j���[�V����
                p.use_tile += p.animation_speed * elapsed_time;
                p.use_tile = p.use_tile * step(p.use_tile, p.tiles.z);

                //�T�u�G�~�b�^�[�ݒ�
                if (p.sub_emitter_flag == 2)
                {
                    p.appearance_timer -= elapsed_time;
                
                    if (p.appearance_timer <= 0)
                    {
                        p.appearance_timer += p.appearance_frequency;
                    
                        float rand_factor = p.velocity.x + p.velocity.y * 21.989 + p.velocity.z * 37.741 + Progress_time;
                    
                        [unroll]
                        for (uint i = 0; i < 1; ++i)
                        {
                            particle set = SetParticle(survival_emitter_data, rand_factor + i, rand_factor);
                            set.position = p.position;
                            set.color = p.color;
                    
                            append_particle_buffer.Append(set);
                        }
                    }
                }
            }
            particle_buffer[id] = p;
        }
    }
}