#include "Stage/StageManager.h"

//更新処理
void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages)
    {
        stage->Update(elapsedTime);
    }
}

//描画処理
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Stage* stage : stages)
    {
        stage->Render(dc, shader);
    }
}

//ステージ登録
void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

//ステージ全削除
void StageManager::Clear()
{
    for (Stage* stage : stages)
    {
        if (stage != nullptr) { delete stage; }
    }
}

//レイキャスト
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

//スフィアキャスト
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