#pragma once

#include "Graphics/Model/Model.h"
#include "Stage/Stage.h"

//ステージ
class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain() override;

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
    DirectX::XMFLOAT3 scale = { 2.0f,2.0f,2.0f };
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

    std::unique_ptr<Model> model;
};