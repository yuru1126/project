#pragma once
#include <vector>
#include <memory>
#include "Particle/Particle.h"

//パーティクル管理クラス
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

    //パーティクル削除
    void Clear();

    spread_particle* GetParticles() const { return particles; }
    void SetParticles(spread_particle* particle) { particles = particle; }

    spread_particle* GetFluidParticles() const { return fluid_particles; }
    void SetFluidParticles(spread_particle* particle) { fluid_particles = particle; }
private:
    spread_particle* particles = nullptr;

    //流体シミュレーション用パーティクル
    spread_particle* fluid_particles = nullptr;
};