#pragma once
#include <DirectXMath.h>
class Camera
{
private:
	Camera()
	{
	}

	~Camera()
	{
	}
public:
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	// 指定方向を向く
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// パースペクティブ設定
	void SetPerspectFov(float fovY,float aspect,float nearZ,float farZ);
	
	// ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// 視点取得
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	// 注視点取得
	const DirectX::XMFLOAT3& GetFocue() const { return focue; }

	// 上方向取得
	const DirectX::XMFLOAT3& GetUp() const { return up; }

	// 前方向取得
	const DirectX::XMFLOAT3& GetFront() const { return front; }

	// 右方向取得
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	// 近クリップ距離取得
	const float& GetNearZ() const { return nearZ; }

	// 遠クリップ距離取得
	const float& GetFarZ() const { return farZ; }

public:
	DirectX::XMFLOAT4X4		view = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	DirectX::XMFLOAT4X4		projection = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

	DirectX::XMFLOAT3		eye = {0,0,0};
	DirectX::XMFLOAT3		focue = { 0,0,0 };

	DirectX::XMFLOAT3		up = { 0,0,0 };
	DirectX::XMFLOAT3		front = { 0,0,0 };
	DirectX::XMFLOAT3		right = { 0,0,0 };

	float					nearZ = 0.1f;
	float					farZ = 2000.0f;
};
