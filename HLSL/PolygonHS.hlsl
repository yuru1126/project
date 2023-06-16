#include "Polygon.hlsli"

HSConstantOutput HSConstant(
	InputPatch<VS_OUT, 3> ip,
	uint pid : SV_PrimitiveID)
{
    //三角形の面積から分割数を変更
    float3 ab = ip[1].position.xyz - ip[0].position.xyz;
    float3 ac = ip[2].position.xyz - ip[0].position.xyz;
    float area = length(cross(ab, ac)) * 0.5;
    
    //平方根を使い値を調整
    area = sqrt(area);
    float divide = max(1, area * multiply);
    
    HSConstantOutput output = (HSConstantOutput) 0;
    output.factor[0] = divide;
    output.factor[1] = divide;
    output.factor[2] = divide;
    output.inner_factor = divide;
    return output;
}

[domain("tri")] //分割に利用する形状を指定　"tri" "quad" "isoline"から選択
[partitioning("integer")] //分割方法 "integer" "fractional_eve" "fractional_odd" "pow2"から選択
[outputtopology("triangle_cw")] //出力された頂点が形成するトポロジー(形状)　"point" "line" "triangle_cw" "triangle_ccw" から選択
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
HS_OUT main(
	InputPatch<VS_OUT, 3> input,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    HS_OUT output = (HS_OUT) 0;
    output.position = input[i].position;
    output.texcoord = input[i].texcoord;
    output.normal = input[i].normal;
    output.color = input[i].color;
    return output;
}
