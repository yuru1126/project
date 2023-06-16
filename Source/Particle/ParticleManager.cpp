#include "Particle/ParticleManager.h"

//パーティクル削除
void ParticleManager::Clear()
{
    if (particles != nullptr)
    {
        delete particles;
        particles = nullptr;
    }

    if (fluid_particles != nullptr)
    {
        delete fluid_particles;
        fluid_particles = nullptr;
    }
}