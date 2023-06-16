#include "Stage/Inheritance/StageMain.h"
#include "Graphics/Model/RegisterModelManager.h"

//�R���X�g���N�^
StageMain::StageMain()
{
    //�X�e�[�W���f����ǂݍ���
    model = std::make_unique<Model>("Data/Model/ExampleStage/ExampleStage.mdl");

    //���f���o�^
    RegisterModelManager::Instance().RegisterModel(model.get());
}

StageMain::~StageMain()
{
    //���f������
    RegisterModelManager::Instance().ExclusionModel(model.get());
}

//�X�V����
void StageMain::Update(float elapsedTime)
{
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);
}

//�`�揈��
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //�V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model.get());
}

void StageMain::UpdateTransform()
{
    transform._11 = scale.x;
    transform._12 = 0.0f;
    transform._13 = 0.0f;
    transform._14 = 0.0f;
    transform._21 = 0.0f;
    transform._22 = scale.y;
    transform._23 = 0.0f;
    transform._24 = 0.0f;
    transform._31 = 0.0f;
    transform._32 = 0.0f;
    transform._33 = scale.z;
    transform._34 = 0.0f;
    transform._41 = 0.0f;
    transform._42 = 0.0f;
    transform._43 = 0.0f;
    transform._44 = 1.0f;
}

//���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModelEST(start, end, model.get(), hit);
}

//�X�t�B�A�L���X�g
bool StageMain::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit)
{
    return Collision::IntersectSphereCastVsModel(start, end, radius, model.get(), hit);
}