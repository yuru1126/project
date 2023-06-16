#include "Particle/Emitter.h"
#include "Particle/ParticleManager.h"
#include "Graphics/Graphics.h"

Emitter::Emitter(spread_particle::SetParticleData data, float inte, int co)
{
    particle_data = data;
    interval = inte;
    interval_timer = inte;
    count = co;
}

//XVˆ—
void Emitter::Update(float elapsedTime)
{
    if (stop)return;

    interval_timer -= elapsedTime;
    if (interval_timer < 0)
    {
        interval_timer = interval;

        spread_particle* particle;
        if (!integrate_fluid) { particle = ParticleManager::Instane().GetParticles(); }
        else { particle = ParticleManager::Instane().GetFluidParticles(); }


        particle->integrate(Graphics::Instance().GetDeviceContext(), particle_data, count);
    }
}

void Emitter::Integrate(int co)
{
    spread_particle* particle;
    if (!integrate_fluid) 
    { 
        particle = ParticleManager::Instane().GetParticles(); 
        particle->integrate(Graphics::Instance().GetDeviceContext(), particle_data, co);
    }
    else 
    {
        particle = ParticleManager::Instane().GetFluidParticles(); 
        particle->integrate(Graphics::Instance().GetDeviceContext(), particle_data, co);
    }
}