#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);
RWStructuredBuffer<float> divergence : register(u2);

[numthreads(10, 10, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    int x = dtid.x + 1;
    int y = dtid.y + 1;
    
    float t = deltaT;
    
    divergence[NArray(x, y)] = (-X_buffer[XNArray(x, y)].velocity - Y_buffer[YNArray(x, y)].velocity +
                X_buffer[XNArray(x + 1, y)].velocity + Y_buffer[YNArray(x, y + 1)].velocity) / t;
}