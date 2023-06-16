#pragma once
#include "Stage/Stage.h"
#include "Graphics/Model/VoronoiModel.h"

//�������{���m�C�X�e�[�W
class VoronoiStage : public Stage
{
public:
    VoronoiStage();
    ~VoronoiStage() override;

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
    DirectX::XMFLOAT3 position = { 0.0f,-0.25f,0.0f };
    DirectX::XMFLOAT3 angle = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3 scale = { 60.0f,1.0f,60.0f };
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

    std::unique_ptr<VoronoiModel> model;
};