#pragma once
#include "Graphics/Shader.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Model/VoronoiModel.h"
#include "Graphics/Model/PolygonModel.h"


//仮でシェーダーをかける用のクラス
class ShaderBox
{
public:
    ShaderBox();
    ~ShaderBox();

    //更新処理
    void Update(float elapsedTime);

    //デバック用GUI描画
    void DrawDebugGUI();

    void SetGeometryType(int type);

    void Register();
    void UnRegister();


    //モデル
    std::unique_ptr<VoronoiModel> Vmodel;
    std::unique_ptr<PolygonModel> Pmodel;

    std::unique_ptr<PolygonModel> PmodelMin;
    std::unique_ptr<PolygonModel> PmodelMax;

    std::unique_ptr<Model> model;

private:
    //行列更新処理
    void UpdateTransform();
    void UpdateTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 an, DirectX::XMFLOAT3 sc);

private:
    DirectX::XMFLOAT3       position = { 0,0,0 };
    DirectX::XMFLOAT3       angle = { 0,0,0 };
    DirectX::XMFLOAT3       scale = { 1,1,1 };
    DirectX::XMFLOAT4X4     transform = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

    DirectX::XMFLOAT3       v_position = { 0,0,0 };
    DirectX::XMFLOAT3       v_angle = { 0,0,0 };
    DirectX::XMFLOAT3       v_scale = { 1,1,1 };

    DirectX::XMFLOAT3       p_position = { 0,0,0 };
    DirectX::XMFLOAT3       p_angle = { 0,0,0 };
    DirectX::XMFLOAT3       p_scale = { 1,1,1 };

    DirectX::XMFLOAT3       p_position_min = { 0,0,0 };
    DirectX::XMFLOAT3       p_position_max = { 0,0,0 };
    DirectX::XMFLOAT3       p_scale_box = { 1,1,1 };


    //ボロノイシェーダー用データ
    VoronoiData v_data;

    //ポリゴンシェーダー用データ
    PolygonData p_data;

    //タイマー
    float timer = 0.0f;

    //ポリゴン用タイマー
    float polgon_timer = 0.0f;
};