#include"DepthShader.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    float depth = pin.depthInView;
    return float4(depth, depth, depth,1.0f);
}