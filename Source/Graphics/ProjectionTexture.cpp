#include "Graphics/ProjectionTexture.h"
#include "imgui.h"

ProjectionTexture::ProjectionTexture()
{
    data.CircleFlag = true;
}

ProjectionTexture::~ProjectionTexture()
{

}

void ProjectionTexture::SetCirclePostion(DirectX::XMFLOAT3 pos)
{
    circle_postion = pos;

    // ˆÊ’u‚ÌXV
    DirectX::XMStoreFloat4x4(&data.circle_transform,
        DirectX::XMMatrixLookAtLH(
            DirectX::XMVectorSet(circle_postion.x, circle_postion.y + upY, circle_postion.z, 1.0f),
            DirectX::XMVectorSet(circle_postion.x, circle_postion.y, circle_postion.z, 1.0f),
            DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f), 
                DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XMConvertToRadians(0), 0))) *
        DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(3), 1.0f, 1.0f, 100.0f));

}

void ProjectionTexture::SetRenderContext(RenderContext* rc)
{
    rc->projection_texture_data = data;
}

void ProjectionTexture::DrawDebugGUI()
{
    if (ImGui::TreeNode("aa"))
    {
        ImGui::SliderFloat3("position", &circle_postion.x, -20.0f, 20.0f);
        ImGui::SliderFloat("upY", &upY, 0.0f, 100.0f);
        ImGui::TreePop();
    }

    SetCirclePostion(circle_postion);
}