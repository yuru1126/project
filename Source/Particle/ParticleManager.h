#pragma once
#include <vector>
#include <memory>
#include "Particle/Particle.h"

//�p�[�e�B�N���Ǘ��N���X
class ParticleManager
{
    ParticleManager() {};
    ~ParticleManager() {};
public:
    static ParticleManager& Instane()
    {
        static	ParticleManager	lightManager;
        return	lightManager;
    }

    //�p�[�e�B�N���폜
    void Clear();

    spread_particle* GetParticles() const { return particles; }
    void SetParticles(spread_particle* particle) { particles = particle; }

    spread_particle* GetFluidParticles() const { return fluid_particles; }
    void SetFluidParticles(spread_particle* particle) { fluid_particles = particle; }
private:
    spread_particle* particles = nullptr;

    //���̃V�~�����[�V�����p�p�[�e�B�N��
    spread_particle* fluid_particles = nullptr;
};