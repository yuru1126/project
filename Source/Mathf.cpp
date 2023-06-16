#include "Mathf.h"
#include <stdlib.h>

//���`���
float Mathf::Lerp(float a,float b,float t)
{
    return a * (1.0f - t) + (b * t);
}

//�w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
    // 0.0�`1.0�̊Ԃ܂ł̃����_���l
    float value = static_cast<float>(rand()) / RAND_MAX;

    // min�`max�܂ł̃����_���l�ɕϊ�
    return min + (max - min) * value;
}

//�O�������O�A��A�E�������擾
void Mathf::GetUpRighi(DirectX::XMFLOAT3 front, DirectX::XMFLOAT3* get_front, DirectX::XMFLOAT3* up, DirectX::XMFLOAT3* right)
{
    DirectX::XMVECTOR Front, Up, Righi;
    //�O�x�N�g�����Z�o
    Front = DirectX::XMVectorSet(front.x, front.y, front.z, 0);
    //�m�[�}���C�Y
    DirectX::XMVector3Normalize(Front);

    //���̏�x�N�g�����Z�o
    //�����^���肸�炵�Ă���
    DirectX::XMVECTOR UP = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    DirectX::XMVector3Normalize(UP);

    using namespace DirectX;
    //�E�x�N�g�����Z�o(�O��)
    Righi = DirectX::XMVector3Cross(UP, Front);
    //��x�N�g�����Z�o
    Up = DirectX::XMVector3Cross(Front, Righi);

    //�v�Z���ʂ����o��
    DirectX::XMStoreFloat3(right, Righi);
    DirectX::XMStoreFloat3(up, Up);
    DirectX::XMStoreFloat3(get_front, Front);
}