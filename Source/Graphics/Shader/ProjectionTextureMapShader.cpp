#include "Graphics/Graphics.h"
#include "Graphics/Shader/ProjectionTextureMapShader.h"
#include "Misc.h"

ProjectionTextureMapShader::ProjectionTextureMapShader(ID3D11Device* device)
{
    // 頂点シェーダー
    {
        create_vs_from_cso(device, "Shader\\ProjectionTextureMapVS.cso",
            vertexShader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
    }

    // ピクセルシェーダー
    {
        create_ps_from_cso(device, "Shader\\ProjectionTextureMapPS.cso",
            pixelShader.ReleaseAndGetAddressOf());
    }

    //定数バッファ
    {
        // シーン用バッファ
        D3D11_BUFFER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.ByteWidth = sizeof(CbScene);
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        //投影テクスチャ用バッファ
        desc.ByteWidth = sizeof(ProjectionTextureData);

        hr = device->CreateBuffer(&desc, 0, textureMapConstantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // ブレンドステート
    {
        D3D11_BLEND_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // 深度ステンシルステート
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // ラスタライザーステート
    {
        D3D11_RASTERIZER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = true;
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.AntialiasedLineEnable = false;

        HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // サンプラステート
    {
        D3D11_SAMPLER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        desc.BorderColor[0] = 1.0f;
        desc.BorderColor[1] = 1.0f;
        desc.BorderColor[2] = 1.0f;
        desc.BorderColor[3] = 1.0f;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

        HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.BorderColor[0] = 0;
        desc.BorderColor[1] = 0;
        desc.BorderColor[2] = 0;
        desc.BorderColor[3] = 0;
        device->CreateSamplerState(&desc, samplerState2.GetAddressOf());
    }
}

void ProjectionTextureMapShader::Begin(const RenderContext& rc)
{
    ID3D11Buffer* constantBuffers[] =
    {
        sceneConstantBuffer.Get(),
        textureMapConstantBuffer.Get()
    };
    rc.deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
    rc.deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);


    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader.Get(), nullptr, 0);
    dc->IASetInputLayout(nullptr);

    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
    dc->RSSetState(rasterizerState.Get());

    ID3D11SamplerState* samplerStates[] =
    {
        samplerState.Get(),
        samplerState2.Get()
    };
    dc->PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);



    // シーン用定数バッファ更新
    CbScene cbScene;
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
    DirectX::XMStoreFloat4x4(&cbScene.inverseViewProjection, XMMatrixInverse(nullptr, V * P));
    DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

    rc.deviceContext->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

void ProjectionTextureMapShader::Draw(const RenderContext& rc, const Sprite* sprite)
{
    ProjectionTextureData data;
    data = rc.projection_texture_data;
    rc.deviceContext->UpdateSubresource(textureMapConstantBuffer.Get(), 0, 0, &data, 0, 0);

    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

    dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

    ID3D11ShaderResourceView* srvs[] =
    {
        sprite->GetShaderResourceView().Get(),
    };
    dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
    dc->Draw(4, 0);
}

void ProjectionTextureMapShader::End()
{
    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
}