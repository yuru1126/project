#pragma once

#include <DirectXMath.h>

class CameraController
{
public:
    CameraController() {};
    ~CameraController() {};

    //更新処理
    void Update(float elapsedTime);

    //操作式更新処理
    void UpdateOperate(float elapsedTime);

    void Reset();
    
    void SetTarget(const DirectX::XMFLOAT3 t) { this->target = t; }
    const DirectX::XMFLOAT3 GetAngle() { return angle; }

private:
    DirectX::XMFLOAT3 target = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    float rollSpeed = DirectX::XMConvertToRadians(45);
    float range = 1.15f;
    //float range = 7.15f;

    float maxAngleX = DirectX::XMConvertToRadians(35);
    float minAngleX = DirectX::XMConvertToRadians(-35);

    bool fast = true;
    //前回の位置
    DirectX::XMFLOAT3 last_time_eye = { 0,0,0 };


    //操作関連の変数
    float					rotateX = 0.0f;
    float					rotateY = 0.0f + DirectX::XMConvertToRadians(180);
    float					distance = 5.0f;
    DirectX::XMFLOAT3 camera_position{ 0.0f, 0.5f, -2.0f };
    DirectX::XMFLOAT3 camera_focus{ 0.0f, 0.5f, 0.0f };

};