#pragma once
#include "Graphics/Shader.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Model/VoronoiModel.h"
#include "Graphics/Model/PolygonModel.h"


//���ŃV�F�[�_�[��������p�̃N���X
class ShaderBox
{
public:
    ShaderBox();
    ~ShaderBox();

    //�X�V����
    void Update(float elapsedTime);

    //�f�o�b�N�pGUI�`��
    void DrawDebugGUI();

    void SetGeometryType(int type);

    void Register();
    void UnRegister();


    //���f��
    std::unique_ptr<VoronoiModel> Vmodel;
    std::unique_ptr<PolygonModel> Pmodel;

    std::unique_ptr<PolygonModel> PmodelMin;
    std::unique_ptr<PolygonModel> PmodelMax;

    std::unique_ptr<Model> model;

private:
    //�s��X�V����
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


    //�{���m�C�V�F�[�_�[�p�f�[�^
    VoronoiData v_data;

    //�|���S���V�F�[�_�[�p�f�[�^
    PolygonData p_data;

    //�^�C�}�[
    float timer = 0.0f;

    //�|���S���p�^�C�}�[
    float polgon_timer = 0.0f;
};