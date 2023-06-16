#include "Graphics/Graphics.h"
#include "Graphics/Shader/Shader3D.h"
#include "Misc.h"


Shader3D::Shader3D(ID3D11Device* device)
{
	DomainShaderEnable = false;

	// 定数バッファ
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

		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbMesh);

		hr = device->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbSubset);

		hr = device->CreateBuffer(&desc, 0, subsetConstantBuffer.GetAddressOf());
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
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
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
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

void Shader3D::Load(const char* VS, const char* PS, const char* GS, const char* HS, const char* DS)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	// 頂点シェーダー
	{
		// ファイルを開く
		FILE* fp = nullptr;
		char filename[256];
		sprintf_s(filename, 256, "Shader/%s.cso", VS);

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

		create_vs_from_cso(device, filename,
			vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	}

	// ピクセルシェーダー
	{
		FILE* fp = nullptr;
		char filename[256];
		sprintf_s(filename, 256, "Shader/%s.cso", PS);

		create_ps_from_cso(device, filename, pixelShader.ReleaseAndGetAddressOf());
	}

	// ジオメトリシェーダー
	if (GS)
	{
		FILE* fp = nullptr;
		char filename[256];
		sprintf_s(filename, 256, "Shader/%s.cso", GS);

		create_gs_from_cso(device, filename, geometryShader.ReleaseAndGetAddressOf());
	}

	// ハルシェーダー
	if (HS)
	{
		FILE* fp = nullptr;
		char filename[256];
		sprintf_s(filename, 256, "Shader/%s.cso", HS);

		create_hs_from_cso(device, filename, hullShader.ReleaseAndGetAddressOf());
	}

	// ドメインシェーダー
	if (DS)
	{
		DomainShaderEnable = true;
		
		FILE* fp = nullptr;
		char filename[256];
		sprintf_s(filename, 256, "Shader/%s.cso", DS);

		create_ds_from_cso(device, filename, domainShader.ReleaseAndGetAddressOf());
	}

}


// 描画開始
void Shader3D::Begin(const RenderContext& rc)
{
	{
		ID3D11ShaderResourceView* srvs[] = {
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
	}

	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	rc.deviceContext->GSSetShader(geometryShader.Get(), nullptr, 0);
	rc.deviceContext->HSSetShader(hullShader.Get(), nullptr, 0);
	rc.deviceContext->DSSetShader(domainShader.Get(), nullptr, 0);

	rc.deviceContext->IASetInputLayout(inputLayout.Get());

	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		meshConstantBuffer.Get(),
		subsetConstantBuffer.Get(),
	};
	rc.deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc.deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc.deviceContext->GSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc.deviceContext->HSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc.deviceContext->DSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc.deviceContext->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	rc.deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	rc.deviceContext->RSSetState(rasterizerState.Get());
	ID3D11SamplerState* samplerStates[] =
	{
		samplerState.Get(),
	};
	rc.deviceContext->PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	rc.deviceContext->DSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

	// シーン用定数バッファ更新
	CbScene cbScene;

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
	cbScene.lightDirection = rc.lightDirection;
	cbScene.viewPosition = rc.viewPosition;
	rc.deviceContext->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

// 必要なバッファの設定
void Shader3D::SetBuffers(const RenderContext& rc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
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
	if (!DomainShaderEnable) 
	{
		rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else 
	{
		rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	}
}

// サブセット単位で描画
void Shader3D::DrawSubset(const RenderContext& rc, const ModelResource::Subset& subset)
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
		subset.material->metallic_map.Get(),	//#2022 Slot2
		subset.material->roughness_map.Get(),	//#2022 Slot3
	};
	rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	rc.deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	rc.deviceContext->DrawIndexed(subset.indexCount, subset.startIndex, 0);
}

// 描画終了
void Shader3D::End(const RenderContext& rc)
{
	rc.deviceContext->VSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->PSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->GSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->HSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->DSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->IASetInputLayout(nullptr);

	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr, nullptr };
	rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}
