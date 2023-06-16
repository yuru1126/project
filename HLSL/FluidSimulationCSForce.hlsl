#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{   
    X_buffer[XNArray(3, 3)].velocity += 7.5;
    Y_buffer[YNArray(3, 3)].velocity += 7.5;
}