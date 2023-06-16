#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model/Model.h"
#include "Collison.h"

//ステージ
class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    //更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    //レイキャスト
    virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

    //スフィアキャスト
    virtual bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit) = 0;
};