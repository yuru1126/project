#include "Mathf.h"
#include <stdlib.h>

//線形補間
float Mathf::Lerp(float a,float b,float t)
{
    return a * (1.0f - t) + (b * t);
}

//指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
    // 0.0～1.0の間までのランダム値
    float value = static_cast<float>(rand()) / RAND_MAX;

    // min～maxまでのランダム値に変換
    return min + (max - min) * value;
}

//前方向より前、上、右方向を取得
void Mathf::GetUpRighi(DirectX::XMFLOAT3 front, DirectX::XMFLOAT3* get_front, DirectX::XMFLOAT3* up, DirectX::XMFLOAT3* right)
{
    DirectX::XMVECTOR Front, Up, Righi;
    //前ベクトルを算出
    Front = DirectX::XMVectorSet(front.x, front.y, front.z, 0);
    //ノーマライズ
    DirectX::XMVector3Normalize(Front);

    //仮の上ベクトルを算出
    //少し真上よりずらしておく
    DirectX::XMVECTOR UP = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    DirectX::XMVector3Normalize(UP);

    using namespace DirectX;
    //右ベクトルを算出(外積)
    Righi = DirectX::XMVector3Cross(UP, Front);
    //上ベクトルを算出
    Up = DirectX::XMVector3Cross(Front, Righi);

    //計算結果を取り出し
    DirectX::XMStoreFloat3(right, Righi);
    DirectX::XMStoreFloat3(up, Up);
    DirectX::XMStoreFloat3(get_front, Front);
}