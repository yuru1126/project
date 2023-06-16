// Gbuffer.cpp
#include "Graphics/Graphics.h"
#include "Gbuffer.h"
#include "Graphics/LightManager.h"
#include "Graphics/ProjectionTextureManager.h"
#include "Camera.h"
#include "Misc.h"

static const DirectX::XMFLOAT2 SCREEN{ 1280.0f ,720.0f };

//DOF
static const DirectX::XMFLOAT2 DOF{ 1280.0f / 2,720.0f / 2 };


Gbuffer::Gbuffer()
{
}

Gbuffer::~Gbuffer()
{
}

void Gbuffer::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	UINT width = static_cast<UINT>(graphics.GetScreenWidth());
	UINT height = static_cast<UINT>(graphics.GetScreenHeight());

	ScreenSprite = std::make_unique<Sprite>();

	// ポストエフェクト初期化
	PostEffectShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	PostEffectShader->Load("PostEffectPS");

	CompositeShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	CompositeShader->Load("CompositePS");

	EnvLightShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	EnvLightShader->Load("DeferredEnvironmentPS");

	LightShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	LightShader->Load("DeferredLightPS", true);

	LightShadowShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	LightShadowShader->Load("DeferredLightShadowPS", true);

	// 深度バッファ生成
	depthStencil.release();
	depthStencil = std::make_unique<DepthStencil>(width, height);
	texture_map_depthStencil.release();
	texture_map_depthStencil = std::make_unique<DepthStencil>(width, height, 0);

	// GBuffer生成
	// カラーバッファ作成(Mipmap使用)
	Color = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	// デプスバッファ作成
	//Depth = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32_FLOAT);
	Depth = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32_FLOAT);
	// 法線バッファ作成
	Normal = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	// ３Ｄ座標バッファ作成
	Position = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//Roughness + Metallic
	RM = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	ProjectionDepth = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32_FLOAT);

	// ライトバッファ作成
	LightBuffer = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	// ライトとカラー合成用作成
	Composite = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	/// 定数バッファ生成
	{
		//ライト用バッファ
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CBLightParam);
		desc.StructureByteStride = 0;
		Graphics::Instance().GetDevice()->CreateBuffer(&desc, 0, CBLight.GetAddressOf());

		//シーンバッファ
		desc.ByteWidth = sizeof(CbScene);
		Graphics::Instance().GetDevice()->CreateBuffer(&desc, 0, scene_constant_buffer.GetAddressOf());

		//ブルームバッファ
		desc.ByteWidth = sizeof(CbBloom);
		Graphics::Instance().GetDevice()->CreateBuffer(&desc, 0, bloom_constant_buffer.GetAddressOf());
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		//sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		//sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		//sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		HRESULT hr = Graphics::Instance().GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = Graphics::Instance().GetDevice()->CreateSamplerState(&sampler_desc, sampler_states[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//ブルーム
	{
		float widthF = (graphics.GetScreenWidth());
		float heightF = (graphics.GetScreenHeight());

		bloom_texture_size[0] = { widthF,heightF };
		bloom_texture_size[1] = { widthF / 4,heightF / 4 };
		bloom_texture_size[2] = { widthF / 8,heightF / 8 };
		bloom_texture_size[3] = { widthF / 16,heightF / 16 };
		bloom_texture_size[4] = { widthF / 32,heightF / 32 };

		for (int i = 0; i < 4; i++)
		{
			bloom_texture[i] = std::make_unique<RenderTarget>(bloom_texture_size[i + 1].x, bloom_texture_size[i + 1].y, DXGI_FORMAT_R16G16B16A16_FLOAT);
		}
		bloom_shader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
		bloom_shader->Load("BloomShaderPS", true);

		//DOF
		DOFbloom_texture = std::make_unique<RenderTarget>(
			DOF.x, DOF.y, DXGI_FORMAT_R16G16B16A16_FLOAT);
		DOFbloom_shader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
		DOFbloom_shader->Load("DofBloomShaderPS", true);

	}

	texture_map_shader = std::make_unique<ProjectionTextureMapShader>(Graphics::Instance().GetDevice());
	texture = std::make_unique<Texture>("Data/Sprite/Lock-on.png");
}

void Gbuffer::Activate()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	ID3D11ShaderResourceView* srvs[] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	// MRT(MultipleRenderTargets)設定
	ID3D11RenderTargetView* targets[] = {
		Color->GetRenderTargetView().Get(),
		Depth->GetRenderTargetView().Get(),
		Normal->GetRenderTargetView().Get(),
		Position->GetRenderTargetView().Get(),
		RM->GetRenderTargetView().Get()
	};

	// レンダーターゲットクリア
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float clearMax[4] = { FLT_MAX, FLT_MAX, FLT_MAX, 1.0f };
	float clearZero[4] = { 0, 0, 0, 1.0f };
	dc->ClearRenderTargetView(targets[0], clearColor);
	dc->ClearRenderTargetView(targets[1], clearMax);
	dc->ClearRenderTargetView(targets[2], clearColor);
	dc->ClearRenderTargetView(targets[3], clearZero);
	dc->ClearRenderTargetView(targets[4], clearZero);

	// 深度バッファクリア
	ID3D11DepthStencilView* dsv = depthStencil->GetDepthStencilView().Get();
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	// レンダーターゲット設定
	dc->OMSetRenderTargets(5, targets, dsv);

	// ビューポート設定
	D3D11_VIEWPORT vp = {
		0,0,
		graphics.GetScreenWidth(),
		graphics.GetScreenHeight(),
		0,1 };
	dc->RSSetViewports(1, &vp);

}

void Gbuffer::DepthActivate()
{

	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();


	ID3D11DepthStencilView* dsv = texture_map_depthStencil->GetDepthStencilView().Get();
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	float clearColor[4] = { 1.0f , 1.0f, 1.0f, 1.0f };
	ID3D11RenderTargetView* target = ProjectionDepth->GetRenderTargetView().Get();

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_render_target_view = nullptr;
	// レンダーターゲット設定
	dc->OMSetRenderTargets(1, &target, dsv);

	// ビューポート設定
	D3D11_VIEWPORT vp = {
		0,0,
		graphics.GetScreenWidth(),
		graphics.GetScreenHeight(),
		0,1 };
	dc->RSSetViewports(1, &vp);
}

void Gbuffer::Lighting()
{
	Graphics& graphics = Graphics::Instance();
	float width = graphics.GetScreenWidth();
	float height = graphics.GetScreenHeight();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	//ライトバッファクリア
	float clearColor[4] = { 0,0,0,1 };
	dc->ClearRenderTargetView(LightBuffer->GetRenderTargetView().Get(), clearColor);
	dc->ClearRenderTargetView(Composite->GetRenderTargetView().Get(), clearColor);

	ID3D11RenderTargetView* targets[]{ NULL, NULL, NULL, NULL, NULL };
	targets[0] = LightBuffer->GetRenderTargetView().Get();
	dc->OMSetRenderTargets(5, targets, NULL);

	ID3D11ShaderResourceView* RTV_Color = Color->GetShaderResourceView().Get();
	//Mipmap生成
	dc->GenerateMips(RTV_Color);

	ID3D11ShaderResourceView* RTV_Position = Position->GetShaderResourceView().Get();
	ID3D11ShaderResourceView* RTV_RM = RM->GetShaderResourceView().Get();
	//ColorはSlot5に変更
	dc->PSSetShaderResources(2, 1, &RTV_Position);
	dc->PSSetShaderResources(3, 1, &RTV_RM);
	dc->PSSetShaderResources(5, 1, &RTV_Color);
	//Slot4にDepth登録
	ID3D11ShaderResourceView* RTV_Depth = Depth->GetShaderResourceView().Get();
	dc->PSSetShaderResources(4, 1, &RTV_Depth);

	Camera& camera = Camera::Instance();
	RenderContext rc;
	{
		rc.deviceContext = dc;

		// カメラパラメータ設定
		rc.viewPosition.x = camera.GetEye().x;
		rc.viewPosition.y = camera.GetEye().y;
		rc.viewPosition.z = camera.GetEye().z;
		rc.viewPosition.w = 1;
		
		rc.view = camera.GetView();
		rc.projection = camera.GetProjection();
	}


	ScreenSprite->SetShaderResourceView(Normal->GetShaderResourceView(), (int)width, (int)height);
	ScreenSprite->Update(
		0, 0, width, height,
		0, 0, width, height,
		0,
		1, 1, 1, 1);

	//環境光
	{
		CbScene scene;
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
		DirectX::XMStoreFloat4x4(&scene.viewProjection, V * P);
		scene.viewPosition = rc.viewPosition;
		dc->UpdateSubresource(scene_constant_buffer.Get(), 0, 0, &scene, 0, 0);
	}
	dc->PSSetConstantBuffers(0, 1, scene_constant_buffer.GetAddressOf());
	EnvLightShader->Begin();
	EnvLightShader->Draw(ScreenSprite.get());
	EnvLightShader->End();

	//ライティング
	dc->PSSetConstantBuffers(1, 1, CBLight.GetAddressOf());
	CBLightParam lightParam{};
	int lightCount = LightManager::Instane().GetLightCount();
	for (int i = -1; i < lightCount; i++) {
		Light* light;
		if (i < 0) 
		{
			light = LightManager::Instane().GetShadowmapLight();
		}
		else 
		{
			light = LightManager::Instane().GetLight(i);
		}
		if (!light) continue;

		switch (light->GetLightType())
		{
		case LightType::Directional:
			lightParam.Color = light->GetColor();
			DirectX::XMFLOAT3 dir = light->GetDirection();
			lightParam.Direction = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
			break;
		case LightType::Point:
			lightParam.Color = light->GetColor();
			DirectX::XMFLOAT3 pos = light->GetPosition();
			lightParam.Direction = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, light->GetRange());
			break;
		case LightType::Spot: continue; //Spotは非対応
		}
		dc->UpdateSubresource(CBLight.Get(), 0, 0, &lightParam, 0, 0);
		if (i >= 0) 
		{
			LightShader->Begin();
			LightShader->Draw(ScreenSprite.get());
			LightShader->End();
		}
		else 
		{
			LightShadowShader->Begin();
			LightShadowShader->Draw(ScreenSprite.get());
			LightShadowShader->End();
		}
	}

	//ライト結果合成
	targets[0] = Composite->GetRenderTargetView().Get();
	dc->OMSetRenderTargets(1, targets, NULL);

	ID3D11ShaderResourceView* RTV_Light = LightBuffer->GetShaderResourceView().Get();
	ID3D11ShaderResourceView* RTV_Normal = Normal->GetShaderResourceView().Get();
	dc->PSSetShaderResources(1, 1, &RTV_Light);
	dc->PSSetShaderResources(4, 1, &RTV_Normal);

	ScreenSprite->SetShaderResourceView(Color->GetShaderResourceView(), (int)width, (int)height);
	ScreenSprite->Update(
		0, 0, width, height,
		0, 0, width, height,
		0,
		1, 1, 1, 1);

	CompositeShader->Begin();
	CompositeShader->Draw(ScreenSprite.get());
	CompositeShader->End();


}


void Gbuffer::Deactivate()
{
	Lighting();

	//ブルーム
	{
		ID3D11DeviceContext* device_context = Graphics::Instance().GetDeviceContext();
		ID3D11RenderTargetView* rtv[1];
		ID3D11DepthStencilView* dsv = nullptr;
		float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
		D3D11_VIEWPORT vp;

		//定数バッファ
		device_context->UpdateSubresource(bloom_constant_buffer.Get(), 0, 0, &cb_bioom, 0, 0);
		device_context->PSSetConstantBuffers(0, 1, bloom_constant_buffer.GetAddressOf());

		bloom_shader->Begin();

		for (int i = 0; i < 4; i++)
		{
			rtv[0] = bloom_texture[i]->GetRenderTargetView().Get();
			// レンダーターゲットを設定
			device_context->OMSetRenderTargets(1, rtv, dsv);
			device_context->ClearRenderTargetView(rtv[0], clearColor);

			DirectX::XMFLOAT2 begin = bloom_texture_size[i];
			DirectX::XMFLOAT2 end = bloom_texture_size[i + 1];


			D3D11_VIEWPORT vp = { 0,0, end.x, end.y, 0,1 };
			device_context->RSSetViewports(1, &vp);
			
			if (i == 0)
			{
				ScreenSprite->SetShaderResourceView(Composite->GetShaderResourceView().Get(), 
					(int)begin.x, (int)begin.y);
			}
			else
			{
				ScreenSprite->SetShaderResourceView(bloom_texture[i - 1]->GetShaderResourceView().Get(), 
					(int)begin.x, (int)begin.y);
			}
			
			ScreenSprite->Update(
				0, 0, end.x, end.y,
				0, 0, begin.x, begin.y,
				0,
				1, 1, 1, 1);
			bloom_shader->Draw(ScreenSprite.get());
		}

		bloom_shader->End();
	}
	return;
}

void Gbuffer::Render()
{
	// Bloom
	ID3D11DeviceContext* device_context = Graphics::Instance().GetDeviceContext();
	ID3D11ShaderResourceView* srvs[] =
	{
		bloom_texture[0]->GetShaderResourceView().Get(),
		bloom_texture[1]->GetShaderResourceView().Get(),
		bloom_texture[2]->GetShaderResourceView().Get(),
		bloom_texture[3]->GetShaderResourceView().Get(),

		//DOF
		DOFbloom_texture->GetShaderResourceView().Get(),
		Depth->GetShaderResourceView().Get(),
		//bloomで空除外
		Normal->GetShaderResourceView().Get(),
	};
	device_context->PSSetShaderResources(1, ARRAYSIZE(srvs), srvs);


	Graphics& graphics = Graphics::Instance();
	float width = graphics.GetScreenWidth();
	float height = graphics.GetScreenHeight();

	ScreenSprite->SetShaderResourceView(Composite->GetShaderResourceView().Get(), (int)width, (int)height);
	ScreenSprite->Update(
		0, 0, width, height,
		0, 0, width, height,
		0,
		1, 1, 1, 1);

	PostEffectShader->Begin();

	device_context->PSSetSamplers(1, 1, sampler_states[0].GetAddressOf());
	device_context->PSSetSamplers(2, 1, sampler_states[1].GetAddressOf());

	PostEffectShader->Draw(ScreenSprite.get());
	PostEffectShader->End();
}

void Gbuffer::DrawDebugGUI()
{
	if (ImGui::TreeNode("GBuffers"))
	{
		ImGui::SliderFloat3("position", &cb_bioom.set_color.x, 0.0, 1.0);
		ImGui::SliderFloat("max", &cb_bioom.max, 0.0, 1.0);
		ImGui::SliderFloat("min", &cb_bioom.min, 0.0, 1.0);
		ImGui::Text("bloom_texture[0]");
		ImGui::Image(bloom_texture[0]->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("bloom_texture[1]");
		ImGui::Image(bloom_texture[1]->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("bloom_texture[2]");
		ImGui::Image(bloom_texture[2]->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("bloom_texture[3]");
		ImGui::Image(bloom_texture[3]->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });


		ImGui::Text("Color");
		ImGui::Image(Color->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("Normal");
		ImGui::Image(Normal->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("Position");
		ImGui::Image(Position->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("Depth");
		ImGui::Image(Depth->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("RM");
		ImGui::Image(RM->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("Light");
		ImGui::Image(LightBuffer->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

		ImGui::Text("bokeh_texture");
		ImGui::Image(DOFbloom_texture->GetShaderResourceView().Get(), { 192, 108 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

		ImGui::TreePop();
	}


}


