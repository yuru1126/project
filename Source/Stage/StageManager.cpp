#include "Stage/StageManager.h"

//�X�V����
void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages)
    {
        stage->Update(elapsedTime);
    }
}

//�`�揈��
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Stage* stage : stages)
    {
        stage->Render(dc, shader);
    }
}

//�X�e�[�W�o�^
void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

//�X�e�[�W�S�폜
void StageManager::Clear()
{
    for (Stage* stage : stages)
    {
        if (stage != nullptr) { delete stage; }
    }
}

//���C�L���X�g
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    HitResult set_hit;
    hit.distance = FLT_MAX;
    for (Stage* stage : stages)
    {
        if (stage->RayCast(start, end, set_hit))
        {
            result = true;

            if (set_hit.distance < hit.distance)
            {
                hit = set_hit;
            }
        }
    }
    return result;
}

//�X�t�B�A�L���X�g
bool StageManager::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit)
{
    bool result = false;
    HitResult set_hit;
    hit.distance = FLT_MAX;
    for (Stage* stage : stages)
    {
        if (stage->SphereCast(start, end, radius, set_hit))
        {
            result = true;

            if (set_hit.distance < hit.distance)
            {
                hit = set_hit;
            }
        }
    }
    return result;
}