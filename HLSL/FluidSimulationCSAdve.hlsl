#include "FluidSimulation.hlsli"

RWStructuredBuffer<Velocity> X_buffer : register(u0);
RWStructuredBuffer<Velocity> Y_buffer : register(u1);

[numthreads(10, 10, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    float t = deltaT;
    
    int x = dtid.x + 1;
    int y = dtid.y + 1;
    float u, v;
    int x_plas = 0;
    int y_plas = 0;
    
    //X方向の更新
    {
        u = X_buffer[XNArray(x, y)].velocity;
        v = (Y_buffer[YNArray(x - 1, y)].velocity + Y_buffer[YNArray(x, y)].velocity +
             Y_buffer[YNArray(x - 1, y + 1)].velocity + Y_buffer[YNArray(x, y + 1)].velocity) * 0.25;
        
        if (u < 0) x_plas = 1;
        if (v < 0) y_plas = 1;
        X_buffer[XNArray(x, y)].after = X_buffer[XNArray(x, y)].velocity -
        u * (X_buffer[XNArray(x + x_plas, y)].velocity - X_buffer[XNArray(x - 1 + x_plas, y)].velocity) * t -
        v * (X_buffer[XNArray(x, y + y_plas)].velocity - X_buffer[XNArray(x, y - 1 + y_plas)].velocity) * t;
    }
    
    x_plas = y_plas = 0;
    
    //Y方向の更新
    {
        u = (X_buffer[XNArray(x, y - 1)].velocity + X_buffer[XNArray(x + 1, y - 1)].velocity +
             X_buffer[XNArray(x + 1, y)].velocity + X_buffer[XNArray(x, y)].velocity) * 0.25;
        v = Y_buffer[YNArray(x, y)].velocity;
        
        if (u < 0) x_plas = 1;
        if (v < 0) y_plas = 1;
        Y_buffer[YNArray(x, y)].after = Y_buffer[YNArray(x, y)].velocity -
        u * (Y_buffer[YNArray(x + x_plas, y)].velocity - Y_buffer[YNArray(x - 1 + x_plas, y)].velocity) * t -
        v * (Y_buffer[YNArray(x, y + y_plas)].velocity - Y_buffer[YNArray(x, y - 1 + y_plas)].velocity) * t;
    }
}