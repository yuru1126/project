#pragma once
#include "Particle/ParticleManager.h"

class Emitter
{
public:
    Emitter() {};
    Emitter(spread_particle::SetParticleData data, float inte = 1.0f / 60.0f, int co = 128);
    ~Emitter() {}

    //�X�V����
    virtual void Update(float elapsedTime);

    //�Q�b�g�A�Z�b�g
    int GetCount() { return count; }
    float GetInterval() { return interval; }

    void SetPosition(DirectX::XMFLOAT4 pos) 
    { 
        particle_data.particle_data.position_max = pos;
        particle_data.particle_data.position_min = pos;
    }
    void SetCount(int co) { count = co; }
    void SetInterval(float inte) { interval = inte; }
    void _SetParticleData(spread_particle::SetParticleData data) { particle_data = data; }
    void SetIntegrateFluid(bool flag) { integrate_fluid = flag; }

    void Stop() { stop = true; }
    void Start() { stop = false; }

    //�o�^
    void Integrate(int co);

protected:
    spread_particle::SetParticleData particle_data;
    float interval = 0.0f;              //���ˊԊu
    float interval_timer = 0.0f;        //���ˊԊu�ϑ��p�^�C�}�[

    int count = 0;                      //���ː�
    bool stop = false;

    //���̓o�^����
    bool integrate_fluid = false;
};