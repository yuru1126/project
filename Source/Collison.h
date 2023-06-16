#pragma once

#include <DirectXMath.h>
#include "Graphics/Model/Model.h"

//�q�b�g����
struct HitResult
{
    DirectX::XMVECTOR	position = { 0, 0, 0 };// ���C�ƃ|���S���̌�_
    DirectX::XMVECTOR	normal = { 0, 0, 0 };	// �Փ˂����|���S���̖@���x�N�g��

    float              distance = 0.0f;      //���C�̎n�_�����_�܂ł̋���
    int                materialIndex = -1;   //�Փ˂����|���S���̃}�e���A���ԍ�
    DirectX::XMFLOAT3 rotation = { 0,0,0 };
    DirectX::XMVECTOR	triangleVerts[3];
};

//AABB
struct AABB
{
    DirectX::XMFLOAT3 center = { 0,0,0 };   //���S
    DirectX::XMFLOAT3 radius = { 0,0,0 };   //���a
};

//�R���W����
class Collision
{
public:
    //���Ƌ��̌�������
    static bool intersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    //�~���Ɖ~���̌�������
    static bool intersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    //���Ɖ~���̌�������
    static bool intersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    //���C�ƃ��f���̌�������
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model*model,
        HitResult& result
    );

    //�y�ʉ�Ver
    static bool IntersectRayVsModelEST(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result);

    //�X�t�B�A�L���X�g�ƃ��f���̌�������  
    static bool IntersectSphereCastVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const float radius,
        const Model* model,
        HitResult& result
    );

    // �O���̓_�ɑ΂���O�p�`�����̍ŋߓ_���Z�o����
    static bool ClosestPosPointTriangle(
        const DirectX::XMVECTOR& Point,
        const DirectX::XMVECTOR TrianglePos[3],
        DirectX::XMVECTOR* nearPos);

    static bool ClosestPosPointTriangle(
        const  DirectX::XMVECTOR& Point,
        const  DirectX::XMVECTOR TrianglePos[3],
        DirectX::XMVECTOR* nearPos,
        DirectX::XMVECTOR* nearTriPos1,
        DirectX::XMVECTOR* nearTriPos2);

    // ���ƎO�p�`�̌�������
    static bool IntersectSphereVsTriangle(
        const DirectX::XMVECTOR& spherePos,
        const float radius,
        const DirectX::XMVECTOR TrianglePos[3],
        DirectX::XMVECTOR* nearPos);	// ���̒��S�ɑ΂���O�p�`�̍ŋߓ_

    // ���C�Ƌ��̌�������
    static bool IntersectRayVsSphere(
        const DirectX::XMVECTOR& start,
        const DirectX::XMVECTOR& end,
        const DirectX::XMVECTOR& spherePos,
        const float radius,
        DirectX::XMVECTOR* hitPos,
        float* hitDistance = {});

    // ���C�Ɖ~���̌�������
    static bool IntersectRayVsCylinder(
        const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end,
        const DirectX::XMVECTOR& startCylinder, const DirectX::XMVECTOR& endCylinder,
        float radius,
        DirectX::XMVECTOR* hitPos,
        float* hitDistance = {});

    // �X�t�B�A�L���X�gVs�O�p�` �ȈՔ�
    static bool IntersectSphereCastVsTriangleEST(
        const DirectX::XMVECTOR& start,
        const DirectX::XMVECTOR& end,
        const float radius,
        const DirectX::XMVECTOR trianglePos[3],
        HitResult& result);

    // �X�t�B�A�L���X�gVs�O�p�` ���S��
    static bool IntersectSphereCastVsTriangle(
        const DirectX::XMVECTOR& start,
        const DirectX::XMVECTOR& end,
        const float radius,
        const DirectX::XMVECTOR trianglePos[3],
        HitResult& result);

private:
    static bool CollideRayVsTriangle(
         DirectX::XMVECTOR& rayStart,  DirectX::XMVECTOR& rayDirection,  float* rayDist,  DirectX::XMVECTOR TrianglePos[3],
        DirectX::XMVECTOR* outPos, DirectX::XMVECTOR* outNormal);

    //�ȈՓI��AABB�̓����蔻��
    static bool AABBVsAABB(AABB first, AABB second, float lenge = 0.0f);
};