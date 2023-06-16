#pragma once
#include <DirectXMath.h>
//浮動少数算術
class Mathf
{
public:
    //線形補正
    static float Lerp(float a,float b,float t);

    //指定範囲のランダム値を計算する
    static float RandomRange(float min, float max);

    //前方向より前、上、右方向を取得
    static void GetUpRighi(DirectX::XMFLOAT3 front, DirectX::XMFLOAT3* get_front, DirectX::XMFLOAT3* up, DirectX::XMFLOAT3* right);
};