#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);

[numthreads(10, 10, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    float t = deltaT;
    
    int x = dtid.x + 1;
    int y = dtid.y + 1;
    
    X_buffer[XNArray(x, y)].after = X_buffer[XNArray(x, y)].velocity - 1.0 / re * (X_buffer[XNArray(x + 1, y)].velocity + X_buffer[XNArray(x, y + 1)].velocity + X_buffer[XNArray(x - 1, y)].velocity + X_buffer[XNArray(x, y - 1)].velocity) * t;
    Y_buffer[YNArray(x, y)].after = Y_buffer[YNArray(x, y)].velocity - 1.0 / re * (Y_buffer[YNArray(x + 1, y)].velocity + Y_buffer[YNArray(x, y + 1)].velocity + Y_buffer[YNArray(x - 1, y)].velocity + Y_buffer[YNArray(x, y - 1)].velocity) * t;
}