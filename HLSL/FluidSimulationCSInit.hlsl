#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);
RWStructuredBuffer<float> divergence : register(u2);
RWStructuredBuffer<float> pressure : register(u3);

[numthreads(13, 12, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    int x = dtid.x;
    int y = dtid.y;
    X_buffer[XNArray(x, y)].velocity = 0;
    X_buffer[XNArray(x, y)].after = 0;
    
    Y_buffer[YNArray(y, x)].velocity = 0;
    Y_buffer[YNArray(y, x)].after = 0;
    
    x = min(x, 11);
    divergence[NArray(x, y)] = 0;
    pressure[NArray(x, y)] = 0;
}