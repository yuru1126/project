#include "FluidSimulation.hlsli"

RWStructuredBuffer<float> divergence : register(u2);
RWStructuredBuffer<float> pressure : register(u3);

[numthreads(1, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    for (int i = 0; i < 100; i++)
    {
        for (int x = 1; x < 11; x++)
        {
            for (int y = 1; y < 11; y++)
            {
                if (x == 1)         pressure[NArray(x - 1, y)] = pressure[NArray(x, y)];
                if (x == 12 - 2)    pressure[NArray(x + 1, y)] = pressure[NArray(x, y)];
                if (y == 1)         pressure[NArray(x, y - 1)] = pressure[NArray(x, y)];
                if (y == 12 - 2)    pressure[NArray(x, y + 1)] = pressure[NArray(x, y)];
                
                pressure[NArray(x, y)] = (1.0 - omega) * pressure[NArray(x, y)] +
                                (omega / 4.0 * (pressure[NArray(x - 1, y)] + pressure[NArray(x + 1, y)] +
                                    pressure[NArray(x, y - 1)] + pressure[NArray(x, y + 1)] - divergence[NArray(x, y)]));
            }
        }
    }
}