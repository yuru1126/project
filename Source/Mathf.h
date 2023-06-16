#pragma once
#include <DirectXMath.h>
//���������Z�p
class Mathf
{
public:
    //���`�␳
    static float Lerp(float a,float b,float t);

    //�w��͈͂̃����_���l���v�Z����
    static float RandomRange(float min, float max);

    //�O�������O�A��A�E�������擾
    static void GetUpRighi(DirectX::XMFLOAT3 front, DirectX::XMFLOAT3* get_front, DirectX::XMFLOAT3* up, DirectX::XMFLOAT3* right);
};