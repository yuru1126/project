#include "DeferredRenderer.h"

#include "Graphics/LightManager.h"
#include "Camera.h"
#include "Particle/ParticleManager.h"

DeferredRenderer::DeferredRenderer(UINT width, UINT height)
{
	//モデルレンダラー初期化
	for (int i = 0; i < static_cast<int>(DEModelRendererId::Max); i++)
	{
		model_renderers[i] = std::make_unique<ModelRenderer>();
	}

	//3Dシェーダー初期化
	voronoi_shader = std::make_unique<VoronoiShader>(Graphics::Instance().GetDevice());
	polygon_shader = std::make_unique<PolygonShader>(Graphics::Instance().GetDevice());
	lambert_shader = std::make_unique<Shader3D>(Graphics::Instance().GetDevice());
	lambert_shader->Load("LambertVS", "LambertPS");

	depth_shader = std::make_unique<Shader3D>(Graphics::Instance().GetDevice());
	depth_shader->Load("DepthShaderVS", "DepthShaderPS");

	// GBuffer初期化
	gbuffer = std::make_unique<Gbuffer>();
	gbuffer->Initialize();

	// 空描画用スプライト
	skyboxRenderer = std::make_unique<Sprite>();
	skybox_shader = std::make_unique<SkyboxShader>(Graphics::Instance().GetDevice());

	// シャドウマップ初期化
	InitializeShadow();

	environment = std::make_unique<HDRTexture>();
	environment->Load("Data/Environment/clarens_midday_4k.hdr");

	return;
}

void DeferredRenderer::Initialize()
{
	environment->SetDesc();
	environment->Set(15);
}

void DeferredRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	// シャドウマップ描画
	RenderShadow();

	// GBufferセット
	gbuffer->Activate();

	// 描画コンテキストに情報をセット
	RenderContext rc{};
	{
		rc.deviceContext = dc;
		rc.screenSize.x = graphics.GetScreenWidth();
		rc.screenSize.y = graphics.GetScreenHeight();
		rc.screenSize.z = camera.GetNearZ();
		rc.screenSize.w = camera.GetFarZ();

		// カメラパラメータ設定
		rc.viewPosition.x = camera.GetEye().x;
		rc.viewPosition.y = camera.GetEye().y;
		rc.viewPosition.z = camera.GetEye().z;
		rc.viewPosition.w = 1;
		rc.view = camera.GetView();
		rc.projection = camera.GetProjection();
	}

	{
		// 空テクスチャを設定
		skyboxRenderer->SetShaderResourceView(environment->GetShaderResourceView().Get(),
			environment->GetWidth(),
			environment->GetHeight());
		skyboxRenderer->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
			0, 0, static_cast<float>(environment->GetWidth()), static_cast<float>(environment->GetHeight()),
			0, 1, 1, 1, 1);
		// 空描画
		skybox_shader->Begin(rc);
		skybox_shader->Draw(rc, skyboxRenderer.get());
		skybox_shader->End(rc);
	}

	//ランバート描画
	lambert_shader->Begin(rc);
	ModelRenderer* model_renderer = model_renderers[static_cast<int>(DEModelRendererId::Default)].get();
	int model_count = model_renderer->GetModelCount();
	for (int i = 0; i < model_count; i++)
	{
		Model* model = model_renderer->GetRenderModel(i);

		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();
		
		model->SetRenderContext(&rc);
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	頂点・インデックスバッファ等設定
			lambert_shader->SetBuffers(rc, nodes, mesh);
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	サブセット単位で描画
				lambert_shader->DrawSubset(rc, subset);
			}
		}
	}
	lambert_shader->End(rc);

	//ボロノイ描画
	voronoi_shader->Begin(rc);
	model_renderer = model_renderers[static_cast<int>(DEModelRendererId::Voronoi)].get();
	model_count = model_renderer->GetModelCount();
	for (int i = 0; i < model_count; i++)
	{
		Model* model = model_renderer->GetRenderModel(i);

		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();

		model->SetRenderContext(&rc);
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	頂点・インデックスバッファ等設定
			voronoi_shader->SetBuffers(rc, nodes, mesh);
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	サブセット単位で描画
				voronoi_shader->DrawSubset(rc, subset);
			}
		}
	}
	voronoi_shader->End(rc);

	//ポリゴン描画
	polygon_shader->Begin(rc);
	model_renderer = model_renderers[static_cast<int>(DEModelRendererId::Polygon)].get();
	model_count = model_renderer->GetModelCount();
	for (int i = 0; i < model_count; i++)
	{
		Model* model = model_renderer->GetRenderModel(i);

		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();

		model->SetRenderContext(&rc);
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	頂点・インデックスバッファ等設定
			polygon_shader->SetBuffers(rc, nodes, mesh);
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	サブセット単位で描画
				polygon_shader->DrawSubset(rc, subset);
			}
		}
	}
	polygon_shader->End(rc);


	//パーティクル描画
	{
		spread_particle* particle = ParticleManager::Instane().GetParticles();
		particle->input(dc, rc);
		particle->render(dc);

		//流体シミュレーション用パーティクル
		spread_particle* fluid_particle = ParticleManager::Instane().GetFluidParticles();
		fluid_particle->input(dc, rc);
		fluid_particle->render(dc);
	}

	//デバッグプリミティブの表示
	if (drawDebugPrimitive)
	{

		// ライトのデバッグプリミティブ描画
		LightManager::Instane().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(rc.deviceContext, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(rc.deviceContext, rc.view, rc.projection);

	}

	// GBufferセット
	gbuffer->Deactivate();

	//	元のバッファに戻す
	RestoreRenderTargets(dc);

	gbuffer->Render();

	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}


void DeferredRenderer::InitializeShadow()
{
	int ShadowMapSize = 1024;
	// カラーバッファ作成
	shadowMap = std::make_unique<RenderTarget>(ShadowMapSize, ShadowMapSize, DXGI_FORMAT_R16G16_FLOAT);
	shadowBlurMap = std::make_unique<RenderTarget>(ShadowMapSize, ShadowMapSize, DXGI_FORMAT_R16G16_FLOAT);
	// 深度バッファ生成
	shadowMapDepth = std::make_unique<DepthStencil>(ShadowMapSize, ShadowMapSize);

	shadowMapShader = std::make_unique<ShadowMapShader>(Graphics::Instance().GetDevice());
	shadowBlurShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	shadowBlurShader->Load("ShadowMapBlurPS");

}

void DeferredRenderer::RenderShadow()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderContext rc{};
	{
		rc.deviceContext = dc;
	}

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	// 画面クリア＆レンダーターゲット設定
	ID3D11DepthStencilView* dsv = shadowMapDepth->GetDepthStencilView().Get();
	float clearColor[] = { FLT_MAX, FLT_MAX, FLT_MAX, 1.0f };
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->ClearRenderTargetView(shadowMap->GetRenderTargetView().Get(), clearColor);

	ID3D11ShaderResourceView* srvs[1] = { NULL };
	rc.deviceContext->PSSetShaderResources(14, 1, srvs);
	dc->OMSetRenderTargets(1, shadowMap->GetRenderTargetView().GetAddressOf(), dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(shadowMap->GetWidth());
	vp.Height = static_cast<float>(shadowMap->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// 登録モデルを描画
	shadowMapShader->Begin(rc);
	for (int i = 0; i < static_cast<int>(DEModelRendererId::Max); i++)
	{
		ModelRenderer* model_renderer = model_renderers[i].get();
		for (const Model* model : model_renderer->GetRenderModels())
		{
			const ModelResource* resource = model->GetResource();
			const std::vector<Model::Node>& nodes = model->GetNodes();
			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				//	頂点・インデックスバッファ等設定
				shadowMapShader->SetBuffers(rc, nodes, mesh);

				//	サブセット単位で描画
				for (const ModelResource::Subset& subset : mesh.subsets)
				{
					shadowMapShader->DrawSubset(rc, subset);
				}
			}
		}
	}
	shadowMapShader->End(rc);

	// ぼかし
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, shadowBlurMap->GetRenderTargetView().GetAddressOf(), dsv);
	vp.Width = static_cast<float>(shadowMap->GetWidth());
	vp.Height = static_cast<float>(shadowMap->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	{
		skyboxRenderer->SetShaderResourceView(shadowMap->GetShaderResourceView().Get(),
			shadowMap->GetWidth(),
			shadowMap->GetHeight());
		skyboxRenderer->Update(
			0, 0, static_cast<float>(shadowMap->GetWidth()), static_cast<float>(shadowMap->GetHeight()),
			0, 0, static_cast<float>(shadowMap->GetWidth()), static_cast<float>(shadowMap->GetHeight()),
			0, 1, 1, 1, 1);
		// 空描画
		shadowBlurShader->Begin(rc);
		shadowBlurShader->Draw(rc, skyboxRenderer.get());
		shadowBlurShader->End(rc);
	}

	//	元のバッファに戻す
	RestoreRenderTargets(dc);

	rc.deviceContext->PSSetShaderResources(14, 1, shadowMap->GetShaderResourceView().GetAddressOf());

}


//	デバッグGUI描画
void DeferredRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("Scene Rendering"))
	{
		ImGui::Checkbox("DebugPrimitive", &drawDebugPrimitive);

		ImGui::Text("ShadowMap");
		ImGui::Image(shadowBlurMap->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

		ImGui::TreePop();
	}
	gbuffer->DrawDebugGUI();

}

//	描画モデル登録全解除
void DeferredRenderer::ClearRenderModel()
{
	for (int i = 0; i < static_cast<int>(DEModelRendererId::Max); i++)
	{
		model_renderers[i]->ClearRenderModel();
	}
}