#include "Stage/Inheritance/VoronoiStage.h"
#include "Graphics/Model/RegisterModelManager.h"

VoronoiStage::VoronoiStage()
{
    //�X�e�[�W���f����ǂݍ���
    model = std::make_unique<VoronoiModel>("Data/Model/Cube/Cube.mdl");
}

VoronoiStage::~VoronoiStage()
{
}

//�X�V����
void VoronoiStage::Update(float elapsedTime)
{
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);
}

//�`�揈��
void VoronoiStage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //�V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model.get());
}

void VoronoiStage::UpdateTransform()
{
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    //��]�s����쐬
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

//���C�L���X�g
bool VoronoiStage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModelEST(start, end, model.get(), hit);
}

//�X�t�B�A�L���X�g
bool VoronoiStage::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit)
{
    return Collision::IntersectSphereCastVsModel(start, end, radius, model.get(), hit);
}