#include "Polygon.hlsli"

[domain("tri")]
DS_OUT main(
	HSConstantOutput input,
	float3 UV : SV_DomainLocation,
	const OutputPatch<HS_OUT, 3> patch)
{
    DS_OUT output = (DS_OUT) 0;
    
    //UV ç¿ïW
    float2 Tex = patch[0].texcoord * UV.x + patch[1].texcoord * UV.y + patch[2].texcoord * UV.z;
    float3 pos = patch[0].position * UV.x + patch[1].position * UV.y + patch[2].position * UV.z;
    float3 Normal = patch[0].normal * UV.x + patch[1].normal * UV.y + patch[2].normal * UV.z;
    
	//èÓïÒê›íË
    output.position = float4(pos, 1.0);
    output.world_position = pos;
    output.texcoord = Tex;
    output.color = patch[0].color;
    output.normal = Normal;
    
    return output;
}
