#include "Stage/Inheritance/StageMain.h"
#include "Graphics/Model/RegisterModelManager.h"

//コンストラクタ
StageMain::StageMain()
{
    //ステージモデルを読み込み
    model = std::make_unique<Model>("Data/Model/ExampleStage/ExampleStage.mdl");

    //モデル登録
    RegisterModelManager::Instance().RegisterModel(model.get());
}

StageMain::~StageMain()
{
    //モデル解除
    RegisterModelManager::Instance().ExclusionModel(model.get());
}

//更新処理
void StageMain::Update(float elapsedTime)
{
    UpdateTransform();

    //モデル行列更新
    model->UpdateTransform(transform);
}

//描画処理
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //シェーダーにモデルを描画してもらう
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

//レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModelEST(start, end, model.get(), hit);
}

//スフィアキャスト
bool StageMain::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit)
{
    return Collision::IntersectSphereCastVsModel(start, end, radius, model.get(), hit);
}