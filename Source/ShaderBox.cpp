#include "ShaderBox.h"
#include <imgui.h>
#include "Input/Input.h"
#include "Graphics/Model/RegisterModelManager.h"
#include "Mathf.h"

ShaderBox::ShaderBox()
{
    model = std::make_unique<Model>("Data/Model/Cube/Cube.mdl");

    //ボロノイデータ初期化
    v_data.lenge = { 0.1f,0.1f,0.05f,0.0f };
    v_data.time_line = { 4.0f,1.0f,0.0f,0.0f };
    Vmodel = std::make_unique<VoronoiModel>("Data/Model/Cube/Cube.mdl");
    Vmodel->SetVoronoiData(v_data);

    //ポリゴンデータ初期化
    p_data.time = 0.0f;
    p_data.destruction_factor = 0.0f;
    p_data.position_factor = 0.2f;
    p_data.rotation_factor = 0.2f;
    p_data.scale_factor = 1.0f;
    p_data.yplas_factor = 0.1f;
    p_data.multiply = 120.0f;

    //ポリゴンJummo設定
    p_data.vertical_end = 1.15f;
    p_data.model_position = { 0.0f,0.25f,1.0f-3 };
    p_data.interpolation_vertical = { 2.75f,-0.25f };
    p_data.area_size_vertical = { 1.5f,0.9f };


    p_data.geometry_type = Geometr_type::BIRTH;
    Pmodel = std::make_unique<PolygonModel>("Data/Model/Jummo/Jummo.mdl");
    PmodelMin = std::make_unique<PolygonModel>("Data/Model/Box/BoxA.mdl");//最低四角
    PmodelMax = std::make_unique<PolygonModel>("Data/Model/Box/BoxB.mdl");//多分割四角
    Pmodel->SetPolygonData(p_data);

    Pmodel->PlayAnimation(0, 1, 0);
    Pmodel->UpdateAnimation(0.01f);


    scale.x = scale.z = scale.y = 0.5f;
    position.z = -20.0f + 25;
    position.x = 2.0f;
    position.y = 1.0f;

    //ボロノイ
    v_scale.x = v_scale.z = v_scale.y = 0.5f;
    v_position.z = -20.0f + 25;
    v_position.x = -2.0f;
    v_position.y = 1.0f;

    //ポリゴン
    p_position = p_data.model_position;
    p_position_min = { 0.0,0.45f,1 };
    p_data.model_position;
    PmodelMin->SetPolygonData(p_data);

    p_position_max = { 0.8f,0.45f,1.0f };
    p_data.model_position;
    PmodelMax->SetPolygonData(p_data);


    p_scale.x = p_scale.z = p_scale.y = 0.005f;
    p_scale_box.x = p_scale_box.z = p_scale_box.y = 0.005f;
    p_angle.y = DirectX::XMConvertToRadians(180);
}

ShaderBox::~ShaderBox() 
{
    //モデル解除
    RegisterModelManager::Instance().ExclusionModel(model.get());
    RegisterModelManager::Instance().VoronoiExclusionModel(Vmodel.get());
    RegisterModelManager::Instance().PolygonExclusionModel(Pmodel.get());
    RegisterModelManager::Instance().PolygonExclusionModel(PmodelMin.get());
    RegisterModelManager::Instance().PolygonExclusionModel(PmodelMax.get());
}

void ShaderBox::Register()
{
    RegisterModelManager::Instance().PolygonRegisterModel(Pmodel.get());
}

void ShaderBox::UnRegister()
{
    RegisterModelManager::Instance().PolygonExclusionModel(Pmodel.get());
}

//更新処理
void ShaderBox::Update(float elapsedTime)
{
    timer += elapsedTime;

    if (p_data.geometry_type <= 1)
    {
        polgon_timer += elapsedTime;
        if (polgon_timer >= 4.0f)
        {
            polgon_timer = 0.0f;
        }

    }
    p_data.time = polgon_timer;

    v_data.lenge.w = timer;
    Vmodel->SetVoronoiData(v_data);

    p_data.model_position = p_position;
    Pmodel->SetPolygonData(p_data);
    p_data.model_position = p_position_min;
    PmodelMin->SetPolygonData(p_data);
    p_data.model_position = p_position_max;
    PmodelMax->SetPolygonData(p_data);

    //行列更新
    UpdateTransform();
    //モデル行列更新
    model->UpdateTransform(transform);
    UpdateTransform(v_position, v_angle, v_scale);
    Vmodel->UpdateTransform(transform);
    UpdateTransform(p_position, p_angle, p_scale);
    Pmodel->UpdateTransform(transform);

    UpdateTransform(p_position_min, p_angle, p_scale);
    PmodelMin->UpdateTransform(transform);
    UpdateTransform(p_position_max, p_angle, p_scale);
    PmodelMax->UpdateTransform(transform);

}

//行列更新処理
void ShaderBox::UpdateTransform()
{
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    //回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}
void ShaderBox::UpdateTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 an, DirectX::XMFLOAT3 sc)
{
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sc.x, sc.y, sc.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(an.x, an.y, an.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    DirectX::XMMATRIX W = S * R * T;
    DirectX::XMStoreFloat4x4(&transform, W);
}

//デバック用GUI描画
void ShaderBox::DrawDebugGUI()
{
    {
        //ポリゴン用
        switch (p_data.geometry_type)
        {
        case Geometr_type::DEATH:
            ImGui::SliderFloat("polgon_timer", &polgon_timer, 0.0, 4.0);
            ImGui::SliderFloat("PositionFactor", &p_data.position_factor, 0.0, 10.0);
            ImGui::SliderFloat("RotationFactor", &p_data.rotation_factor, 0.0, 1.0);
            ImGui::SliderFloat("ScaleFactor", &p_data.scale_factor, 0.0, 1.0);
            ImGui::SliderFloat("YPlasFactor", &p_data.yplas_factor, 0.0, 1.0);
            break;
        case Geometr_type::BIRTH:
            ImGui::SliderFloat("polgon_timer", &polgon_timer, 0.0, 4.0);
            
            break;
        case Geometr_type::DEATH_NOT_TIMER:
            ImGui::SliderFloat("Destruction", &p_data.destruction_factor, 0.0, 1.0);
            ImGui::SliderFloat("PositionFactor", &p_data.position_factor, 0.0, 10.0);
            ImGui::SliderFloat("RotationFactor", &p_data.rotation_factor, 0.0, 1.0);
            ImGui::SliderFloat("ScaleFactor", &p_data.scale_factor, 0.0, 1.0);
            ImGui::SliderFloat("YPlasFactor", &p_data.yplas_factor, 0.0, 1.0);

            polgon_timer = Mathf::Lerp(1.0f, 2.5f, p_data.destruction_factor);

            break;
        case Geometr_type::BIRTH_NOT_TIMER:
            ImGui::SliderFloat("Destruction", &p_data.destruction_factor, 0.0, 1.0);
            break;
        }

        //面積のかけ算
        ImGui::SliderFloat("Multiply", &p_data.multiply, 0.0, 120.0);
    }
}

void ShaderBox::SetGeometryType(int type)
{
    p_data.geometry_type = type;
    p_data.destruction_factor = 0.0f;
    switch (type)
    {
    case Geometr_type::DEATH:
        polgon_timer = 0.0f;
        break;
    case Geometr_type::BIRTH:
        polgon_timer = 0.0f;
        break;
    case Geometr_type::DEATH_NOT_TIMER:
        polgon_timer = 0.0f;
        break;
    case Geometr_type::BIRTH_NOT_TIMER:
        polgon_timer = 5.0f;
        break;

    }
}