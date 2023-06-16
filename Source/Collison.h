#pragma once

#include <DirectXMath.h>
#include "Graphics/Model/Model.h"

//ヒット結果
struct HitResult
{
    DirectX::XMVECTOR	position = { 0, 0, 0 };// レイとポリゴンの交点
    DirectX::XMVECTOR	normal = { 0, 0, 0 };	// 衝突したポリゴンの法線ベクトル

    float              distance = 0.0f;      //レイの始点から交点までの距離
    int                materialIndex = -1;   //衝突したポリゴンのマテリアル番号
    DirectX::XMFLOAT3 rotation = { 0,0,0 };
    DirectX::XMVECTOR	triangleVerts[3];
};

//AABB
struct AABB
{
    DirectX::XMFLOAT3 center = { 0,0,0 };   //中心
    DirectX::XMFLOAT3 radius = { 0,0,0 };   //半径
};

//コリジョン
class Collision
{
public:
    //球と球の交差判定
    static bool intersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    //円柱と円柱の交差判定
    static bool intersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    //球と円柱の交差判定
    static bool intersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    //レイとモデルの交差判定
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model*model,
        HitResult& result
    );

    //軽量化Ver
    static bool IntersectRayVsModelEST(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result);

    //スフィアキャストとモデルの交差判定  
    static bool IntersectSphereCastVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const float radius,
        const Model* model,
        HitResult& result
    );

    // 外部の点に対する三角形内部の最近点を算出する
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

    // 球と三角形の交差判定
    static bool IntersectSphereVsTriangle(
        const DirectX::XMVECTOR& spherePos,
        const float radius,
        const DirectX::XMVECTOR TrianglePos[3],
        DirectX::XMVECTOR* nearPos);	// 球の中心に対する三角形の最近点

    // レイと球の交差判定
    static bool IntersectRayVsSphere(
        const DirectX::XMVECTOR& start,
        const DirectX::XMVECTOR& end,
        const DirectX::XMVECTOR& spherePos,
        const float radius,
        DirectX::XMVECTOR* hitPos,
        float* hitDistance = {});

    // レイと円柱の交差判定
    static bool IntersectRayVsCylinder(
        const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end,
        const DirectX::XMVECTOR& startCylinder, const DirectX::XMVECTOR& endCylinder,
        float radius,
        DirectX::XMVECTOR* hitPos,
        float* hitDistance = {});

    // スフィアキャストVs三角形 簡易版
    static bool IntersectSphereCastVsTriangleEST(
        const DirectX::XMVECTOR& start,
        const DirectX::XMVECTOR& end,
        const float radius,
        const DirectX::XMVECTOR trianglePos[3],
        HitResult& result);

    // スフィアキャストVs三角形 完全版
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

    //簡易的なAABBの当たり判定
    static bool AABBVsAABB(AABB first, AABB second, float lenge = 0.0f);
};