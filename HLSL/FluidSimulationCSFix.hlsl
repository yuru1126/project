#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);
RWStructuredBuffer<float> pressure : register(u3);

[numthreads(10, 10, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    int x = dtid.x + 1;
    int y = dtid.y + 1;
    
    float t = deltaT;
    X_buffer[XNArray(x, y)].velocity -= (pressure[NArray(x, y)] - pressure[NArray(x - 1, y)]) * t;
    Y_buffer[YNArray(x, y)].velocity -= (pressure[NArray(x, y)] - pressure[NArray(x, y - 1)]) * t;
}