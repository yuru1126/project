#include"ProjectionTextureMap.hlsli"

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout;
    //頂点を画面いっぱいに設定(NDC空間)
    const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
    const float2 texcoords[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    vout.position = float4(position[vertexid], 0, 1);
    vout.texcoord = texcoords[vertexid];
    return vout;
}