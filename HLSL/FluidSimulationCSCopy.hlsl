#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);

[numthreads(10, 10, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    int x = dtid.x + 1;
    int y = dtid.y + 1;
    
    X_buffer[XNArray(x, y)].velocity = X_buffer[XNArray(x, y)].after;
    Y_buffer[YNArray(x, y)].velocity = Y_buffer[YNArray(x, y)].after;
}