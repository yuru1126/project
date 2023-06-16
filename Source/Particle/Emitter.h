#pragma once
#include "Particle/ParticleManager.h"

class Emitter
{
public:
    Emitter() {};
    Emitter(spread_particle::SetParticleData data, float inte = 1.0f / 60.0f, int co = 128);
    ~Emitter() {}

    //更新処理
    virtual void Update(float elapsedTime);

    //ゲット、セット
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

    //登録
    void Integrate(int co);

protected:
    spread_particle::SetParticleData particle_data;
    float interval = 0.0f;              //発射間隔
    float interval_timer = 0.0f;        //発射間隔観測用タイマー

    int count = 0;                      //発射数
    bool stop = false;

    //流体登録判定
    bool integrate_fluid = false;
};