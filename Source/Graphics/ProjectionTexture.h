#pragma once
#include <DirectXMath.h>
#include "Graphics/RenderContext.h"

class ProjectionTexture
{
public:
    ProjectionTexture();
    ~ProjectionTexture();

    void SetCirclePostion(DirectX::XMFLOAT3 pos);

    void SetRenderContext(RenderContext* rc);

    void DrawDebugGUI();

private:
    DirectX::XMFLOAT3 circle_postion = { 0,0,0 };
    float upY = +100.0f;
    ProjectionTextureData data;
};