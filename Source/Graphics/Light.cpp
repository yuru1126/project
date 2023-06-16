#include	"Graphics/Graphics.h"
#include	"Light.h"

Light::Light(LightType lightType)
	: lightType(lightType)
{
}

void Light::DrawDebugGUI()
{
	static constexpr char* lightTypeName[] =
	{
		"Directional",
		"Point",
		"Spot",
	};

	if (ImGui::TreeNode(lightTypeName[static_cast<int>(lightType)]))
	{
		switch (lightType)
		{
		case	LightType::Directional:
		{
			if (ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f))
			{
				float l = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
				direction.x /= l;
				direction.y /= l;
				direction.z /= l;
			}
			ImGui::ColorEdit3("color", &color.x);
			ImGui::DragFloat("intensity", &color.w, 0.1f, 0, FLT_MAX);
			break;
		}
		case	LightType::Point:
		{
			ImGui::DragFloat3("position", &position.x);
			ImGui::ColorEdit3("color", &color.x);
			ImGui::DragFloat("intensity", &color.w, 0.1f, 0, FLT_MAX);
			ImGui::DragFloat("range", &range, 0.1f, 0, FLT_MAX);
			break;
		}
		case LightType::Spot:
		{
			ImGui::DragFloat3("position", &position.x);
			ImGui::DragFloat3("direction", &direction.x, 0.01f);
			DirectX::XMStoreFloat3(&direction,
				DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)));
			ImGui::ColorEdit3("color", &color.x);
			ImGui::DragFloat("intensity", &color.w, 0.1f, 0, FLT_MAX);
			ImGui::DragFloat("range", &range, 0.1f, 0, FLT_MAX);
			ImGui::SliderFloat("innerCorn", &innerCorn, 0, 1.0f);
			ImGui::SliderFloat("outerCorn", &outerCorn, 0, 1.0f);
			break;
		}
		}
		ImGui::TreePop();
	}
}

void Light::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();

	switch (lightType)
	{
	case LightType::Directional:
	{
		//	平行光源は表示しない。
		break;
	}
	case LightType::Point:
	{
		//	点光源は全方位に光を放射する光源なので球体を描画する。
		debugRenderer->DrawSphere(position, range, color);
		break;
	}
	case LightType::Spot:
	{
		DirectX::XMVECTOR	Direction = DirectX::XMLoadFloat3(&direction);
		float len;
		DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(Direction));
		if (len <= 0.00001f)
			break;
		Direction = DirectX::XMVector3Normalize(Direction);

		//	軸算出
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, Direction);
		DirectX::XMVECTOR Work = fabs(dir.y) == 1 ? DirectX::XMVectorSet(1, 0, 0, 0)
			: DirectX::XMVectorSet(0, 1, 0, 0);
		DirectX::XMVECTOR	XAxis = DirectX::XMVector3Cross(Direction, Work);
		DirectX::XMVECTOR	YAxis = DirectX::XMVector3Cross(XAxis, Direction);
		XAxis = DirectX::XMVector3Cross(Direction, YAxis);

		static constexpr int SplitCount = 16;
		for (int u = 0; u < SplitCount; u++)
		{
			float s = static_cast<float>(u) / static_cast<float>(SplitCount);
			float r = -DirectX::XM_PI + DirectX::XM_2PI * s;
			// 回転行列算出
			DirectX::XMMATRIX	RotationZ = DirectX::XMMatrixRotationAxis(Direction, r);
			// 線を算出
			DirectX::XMFLOAT3	OldPoint;
			{
				DirectX::XMVECTOR	Point = Direction;
				DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(outerCorn))
					* RotationZ;
				Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(range, range, range, 0));
				Point = DirectX::XMVector3TransformCoord(Point, Rotation);
				Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&position));
				DirectX::XMFLOAT3	pos;
				DirectX::XMStoreFloat3(&pos, Point);
				lineRenderer->AddVertex(position, color);
				lineRenderer->AddVertex(pos, color);
				OldPoint = pos;
			}
			// 球面を算出
			for (int v = 0; v <= SplitCount; ++v)
			{
				float s = static_cast<float>(v) / static_cast<float>(SplitCount);
				float a = outerCorn + (1.0f - outerCorn) * s;
				DirectX::XMVECTOR	Point = Direction;
				DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(a))
					* RotationZ;
				Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(range, range, range, 0));
				Point = DirectX::XMVector3TransformCoord(Point, Rotation);
				Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&position));
				DirectX::XMFLOAT3	pos;
				DirectX::XMStoreFloat3(&pos, Point);
				lineRenderer->AddVertex(OldPoint, color);
				lineRenderer->AddVertex(pos, color);
				OldPoint = pos;
			}
		}
		break;
	}
	}
}
