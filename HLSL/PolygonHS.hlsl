#include "Polygon.hlsli"

HSConstantOutput HSConstant(
	InputPatch<VS_OUT, 3> ip,
	uint pid : SV_PrimitiveID)
{
    //�O�p�`�̖ʐς��番������ύX
    float3 ab = ip[1].position.xyz - ip[0].position.xyz;
    float3 ac = ip[2].position.xyz - ip[0].position.xyz;
    float area = length(cross(ab, ac)) * 0.5;
    
    //���������g���l�𒲐�
    area = sqrt(area);
    float divide = max(1, area * multiply);
    
    HSConstantOutput output = (HSConstantOutput) 0;
    output.factor[0] = divide;
    output.factor[1] = divide;
    output.factor[2] = divide;
    output.inner_factor = divide;
    return output;
}

[domain("tri")] //�����ɗ��p����`����w��@"tri" "quad" "isoline"����I��
[partitioning("integer")] //�������@ "integer" "fractional_eve" "fractional_odd" "pow2"����I��
[outputtopology("triangle_cw")] //�o�͂��ꂽ���_���`������g�|���W�[(�`��)�@"point" "line" "triangle_cw" "triangle_ccw" ����I��
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
