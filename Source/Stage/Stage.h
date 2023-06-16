#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model/Model.h"
#include "Collison.h"

//�X�e�[�W
class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    //���C�L���X�g
    virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

    //�X�t�B�A�L���X�g
    virtual bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float radius, HitResult& hit) = 0;
};