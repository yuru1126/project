#include "Stage/Inheritance/VoronoiStage.h"
#include "Graphics/Model/RegisterModelManager.h"

VoronoiStage::VoronoiStage()
{
    //ステージモデルを読み込み
    model = std::make_unique<VoronoiModel>("Data/Model/Cube/Cube.mdl");
}

VoronoiStage::~VoronoiStage()
{
}

//更新処理
void VoronoiStage::Update(float elapsedTime)
{
    UpdateTransform();

    //モデル行列更新
    model->UpdateTransform(transform);
}

//描画処理
void VoronoiStage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //シェーダーにモデルを描画してもらう
    shader->Draw(dc, model.get());
}

void VoronoiStage::UpdateTransform()
{
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    //回転行列を作成
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

//レイキャスト
bool VoronoiStage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModelEST(start, end, model.get(), hit);
}

//スフィアキャスト
bool VoronoiStage::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit)
{
    return Collision::IntersectSphereCastVsModel(start, end, radius, model.get(), hit);
}