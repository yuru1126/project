#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);

[numthreads(2, 2, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    int x = dtid.x * (12 - 1);
    int y = dtid.y * (12 - 1);
    
    X_buffer[XNArray(x, y)].velocity = 0.0;
    X_buffer[XNArray(x + 1, y)].velocity = 0.0;
    Y_buffer[YNArray(x, y)].velocity = 0.0;
    Y_buffer[YNArray(x, y + 1)].velocity = 0.0;

}