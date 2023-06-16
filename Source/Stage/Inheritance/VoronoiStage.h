#pragma once
#include "Stage/Stage.h"
#include "Graphics/Model/VoronoiModel.h"

//お試しボロノイステージ
class VoronoiStage : public Stage
{
public:
    VoronoiStage();
    ~VoronoiStage() override;

    //更新処理
    void Update(float elapsedTime) override;

    //描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    //レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

    //スフィアキャスト
    bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit) override;

private:
    //行列更新処理
    void UpdateTransform();
    DirectX::XMFLOAT3 position = { 0.0f,-0.25f,0.0f };
    DirectX::XMFLOAT3 angle = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3 scale = { 60.0f,1.0f,60.0f };
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

    std::unique_ptr<VoronoiModel> model;
};