#include "Misc.h"
#include "Graphics/Shader/ShadowMapShader.h"
#include "Graphics/LightManager.h"
#include "Graphics/Graphics.h"

ShadowMapShader::ShadowMapShader(ID3D11Device* device)
{
	// 頂点シェーダー
	{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device, "Shader/ShadowMapVS.cso",
			vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	}

	// ピクセルシェーダー
	{
		create_ps_from_cso(device, "Shader/ShadowMapPS.cso", pixelShader.ReleaseAndGetAddressOf());
	}

	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbScene);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// メッシュ用バッファ
		desc.ByteWidth = sizeof(CbMesh);

		hr = device->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbSubset);

		hr = device->CreateBuffer(&desc, 0, subsetConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// シャドウマップ用バッファ
		desc.ByteWidth = sizeof(CBShadowMapParam);
		hr = device->CreateBuffer(&desc, 0, CBShadowMap.GetAddressOf());
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
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
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

		//	シャドウマップ用
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.BorderColor[0] = 1000;
		desc.BorderColor[1] = 1000;
		desc.BorderColor[2] = 1000;
		desc.BorderColor[3] = 1000;
		hr = device->CreateSamplerState(&desc, samplerStateShadowMap.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 描画開始
void ShadowMapShader::Begin(const RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = rc.deviceContext;

	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		meshConstantBuffer.Get(),
		subsetConstantBuffer.Get(),
	};
	dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
	dc->RSSetState(rasterizerState.Get());
	dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	ID3D11SamplerState* samplerStates[] =
	{
		samplerState.Get(),
		samplerStateShadowMap.Get()
	};
	dc->PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

	// シーン用定数バッファ更新
	CbScene cbScene;
	{
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
		DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
		cbScene.viewPosition = rc.viewPosition;
		dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
	}
	//シャドウマップ設定

	Light* light = LightManager::Instane().GetShadowmapLight();
	if (!light)
	{
		return;
	}

	// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
	DirectX::XMVECTOR TargetPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&light->GetDirection());
	LightPosition = DirectX::XMVectorScale(LightPosition, -50.0f);
	LightPosition = DirectX::XMVectorAdd(LightPosition, TargetPosition);
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
		TargetPosition,
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(30, 30, 10, 100);
	DirectX::XMMATRIX VP = V * P;

	CBShadowMapParam ShadowMapParam;
	DirectX::XMStoreFloat4x4(&ShadowMapParam.shadowVP, VP);
	dc->UpdateSubresource(CBShadowMap.Get(), 0, 0, &ShadowMapParam, 0, 0);

	dc->VSSetConstantBuffers(10, 1, CBShadowMap.GetAddressOf());
	dc->PSSetConstantBuffers(10, 1, CBShadowMap.GetAddressOf());
}

// 必要なバッファの設定
void ShadowMapShader::SetBuffers(const RenderContext& rc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
{
	// メッシュ用定数バッファ更新
	CbMesh cbMesh;
	::memset(&cbMesh, 0, sizeof(cbMesh));
	if (mesh.nodeIndices.size() > 0)
	{
		for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
		{
			DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
			DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
			DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
			DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[i], boneTransform);
		}
	}
	else
	{
		cbMesh.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
	}
	rc.deviceContext->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	UINT stride = sizeof(ModelResource::Vertex);
	UINT offset = 0;
	rc.deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
	rc.deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// サブセット単位で描画
void ShadowMapShader::DrawSubset(const RenderContext& rc, const ModelResource::Subset& subset)
{
	CbSubset cbSubset;
	cbSubset.materialColor = subset.material->color;
	cbSubset.ka = subset.material->phong.ka;
	cbSubset.kd = subset.material->phong.kd;
	cbSubset.ks = subset.material->phong.ks;
	cbSubset.shiness = subset.material->phong.shiness;
	rc.deviceContext->UpdateSubresource(subsetConstantBuffer.Get(), 0, 0, &cbSubset, 0, 0);
	ID3D11ShaderResourceView* srvs[] =
	{
		subset.material->diffuse_map.Get(),
		subset.material->normal_map.Get(),
		subset.material->metallic_map.Get(),	
		subset.material->roughness_map.Get(),	
	};
	rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	rc.deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	rc.deviceContext->DrawIndexed(subset.indexCount, subset.startIndex, 0);
}

// 描画終了
void ShadowMapShader::End(const RenderContext& rc)
{
	rc.deviceContext->VSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->PSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->IASetInputLayout(nullptr);
}
