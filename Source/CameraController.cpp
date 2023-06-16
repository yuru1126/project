#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include <imgui.h>
#include "Collison.h"
#include "Stage/StageManager.h"
#include "Mathf.h"

//更新処理
void CameraController::Update(float elapsedTime)
{
    GamePad& gamepad = Input::Instance().GetGamePad();
    float ax = gamepad.GetAxisRX();
    float ay = -gamepad.GetAxisRY();
    //カメラの回転速度
    float speed = rollSpeed * elapsedTime;

    //スティックの入力値に合わせてX軸とY軸を回転
    {
        angle.x += ay * speed;
        angle.y += ax * speed;
    }
    
    //X軸のカメラ回転を制限
    if (angle.x > maxAngleX)
    {
        angle.x = maxAngleX;
    }
    if (angle.x < minAngleX)
    {
        angle.x = minAngleX;
    }
    //Y軸の回転値を3.14~-3.14に収まるようにする
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.y > DirectX::XM_PI)
    {
        angle.y -= DirectX::XM_2PI;
    }

    //カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform = 
        DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    //回転行列から前方向ベクトルを取り出す
    DirectX::XMVECTOR Front = Transform.r[2];//行列の３行目
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);
    //視点
    DirectX::XMFLOAT3 eye{};

    eye.x = target.x + -front.x * range;
    eye.y = target.y + -front.y * range;
    eye.z = target.z + -front.z * range;

    //レイキャストを行い壁にめり込んでいればめり込まない位置まで移動させる
    DirectX::XMFLOAT3 start = target;
    DirectX::XMFLOAT3 end = eye;
    //レイキャストによる壁判定
    HitResult hit;
    start.y += 0.005f;
    if (StageManager::Instance().SphereCast(start, end, 0.005f, hit))//もし壁に当たっていれば
    {
        DirectX::XMVECTOR DIR = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&end), DirectX::XMLoadFloat3(&start)));
        DirectX::XMVECTOR POS = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&start), DirectX::XMVectorScale(DIR, hit.distance));
        DirectX::XMStoreFloat3(&eye, POS);

        target.x = eye.x + front.x * range;
        target.y = eye.y + front.y * range;
        target.z = eye.z + front.z * range;
    }

    //カメラの視点と注視点を設定
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0,1,0));
}

//操作式更新処理
void CameraController::UpdateOperate(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    float moveX = (mouse.GetPositionX() - mouse.GetOldPositionX()) * 0.02f;
    float moveY = (mouse.GetPositionY() - mouse.GetOldPositionY()) * 0.02f;

    Camera& camera = Camera::Instance();

    // 視線行列を生成
    DirectX::XMMATRIX V;
    {
        DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
        // マウス操作
        {
            if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
            {
                // Y軸回転
                rotateY += moveX * 0.5f;
                if (rotateY > DirectX::XM_PI) 
                {
                    rotateY -= DirectX::XM_2PI;
                }
                else if (rotateY < -DirectX::XM_PI) 
                {
                    rotateY += DirectX::XM_2PI; 
                }
                
                // X軸回転
                rotateX += moveY * 0.5f;
                if (rotateX > DirectX::XMConvertToRadians(89.9f)) 
                {
                    rotateX = DirectX::XMConvertToRadians(89.9f);
                }
                else if (rotateX < -DirectX::XMConvertToRadians(89.9f)) 
                { 
                    rotateX = -DirectX::XMConvertToRadians(89.9f);
                }
            }
            else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
            {
                V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&camera_position),
                    DirectX::XMLoadFloat3(&camera_focus), up);
                DirectX::XMFLOAT4X4 W;
                DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
                // 平行移動
                float s = distance * 0.035f;
                float x = moveX * s;
                float y = moveY * s;
                camera_focus.x -= W._11 * x;
                camera_focus.y -= W._12 * x;
                camera_focus.z -= W._13 * x;

                camera_focus.x += W._21 * y;
                camera_focus.y += W._22 * y;
                camera_focus.z += W._23 * y;
            }
            if (mouse.GetWheel() != 0)	// ズーム
            {

                distance -= static_cast<float>(mouse.GetWheel()) * distance * 0.001f;
            }
        }
        float sx = ::sinf(rotateX), cx = ::cosf(rotateX);
        float sy = ::sinf(rotateY), cy = ::cosf(rotateY);
        DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera_focus);
        DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
        DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
        Front = DirectX::XMVectorMultiply(Front, Distance);
        DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, Front);
        DirectX::XMStoreFloat3(&camera_position, Eye);
        
        // カメラに視点を注視点を設定
        Camera::Instance().SetLookAt(camera_position, camera_focus, { 0, 1, 0 });
    }
}

void CameraController::Reset()
{
    rotateX = 0.0f;
    rotateY = 0.0f + DirectX::XMConvertToRadians(180);
    distance = 5.0f;
    camera_position = { 0.0f, 0.5f, -2.0f };
    camera_focus = { 0.0f, 0.5f, 0.0f };
}