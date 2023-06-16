#pragma once

#include "Graphics/Model/Model.h"
#include "Stage/Stage.h"

//�X�e�[�W
class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain() override;

    //�X�V����
    void Update(float elapsedTime) override;

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    //���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

    //�X�t�B�A�L���X�g
    bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit) override;

private:
    //�s��X�V����
    void UpdateTransform();
    DirectX::XMFLOAT3 scale = { 2.0f,2.0f,2.0f };
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

    std::unique_ptr<Model> model;
};