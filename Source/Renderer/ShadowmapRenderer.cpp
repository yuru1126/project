#include "Renderer/ShadowmapRenderer.h"
#include "Graphics/LightManager.h"

ShadowmapRenderer::ShadowmapRenderer(UINT shadowmapSize)
{
	Graphics& graphics = Graphics::Instance();
	depthStencil.release();
	depthStencil = std::make_unique<DepthStencil>(shadowmapSize, shadowmapSize);

	shadowmap_caster_shader = std::make_unique<ShadowmapCasterShader>(graphics.GetDevice());

	model_renderer = std::make_unique<ModelRenderer>();
}

ShadowmapRenderer::~ShadowmapRenderer()
{
}

//ShadowMapData ShadowmapRenderer::GetShadowMapData()
//{
//	ShadowMapData shadowMapData;
//	shadowMapData.shadowMap = depthStencil->GetShaderResourceView().Get();
//	DirectX::XMFLOAT4X4 view, projection;
//	CalcShadowmapMatrix(view, projection);
//	DirectX::XMStoreFloat4x4(&shadowMapData.lightViewProjection,
//		DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
//	shadowMapData.shadowBias = shadowBias;
//	shadowMapData.shadowColor = shadowColor;
//	return shadowMapData;
//}

void ShadowmapRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	// 画面クリア＆レンダーターゲット設定
	ID3D11DepthStencilView* dsv = depthStencil->GetDepthStencilView().Get();
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(0, nullptr, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(depthStencil->GetWidth());
	vp.Height = static_cast<float>(depthStencil->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// 描画コンテキストに情報をセット
	RenderContext rc;
	{
		rc.deviceContext = dc;
		rc.screenSize.x = vp.Width;
		rc.screenSize.y = vp.Height;
		rc.screenSize.z = Near;
		rc.screenSize.w = Far;
		CalcShadowmapMatrix(rc.view, rc.projection);
	}

	// 登録モデルを描画
	shadowmap_caster_shader->Begin(rc);
	for (int i = 0; i < model_renderer->GetModelCount(); i++)
	{
		Model* model = model_renderer->GetRenderModel(i);
		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	頂点・インデックスバッファ等設定
			shadowmap_caster_shader->SetBuffers(rc, nodes, mesh);
	
			//	サブセット単位で描画
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				shadowmap_caster_shader->DrawSubset(rc, subset);
			}
		}
	}
	shadowmap_caster_shader->End(rc);

	//	元のバッファに戻す
	RestoreRenderTargets(dc);
}

//	デバッグGUI描画
void ShadowmapRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("Shadowmap Rendering"))
	{
		ImGui::SliderFloat("Rectangle", &shadowRect, 1.0f, 2048.0f);
		ImGui::ColorEdit3("Color", &shadowColor.x);
		ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.1f, "%0.4f");

		if (ImGui::TreeNode("RenderTargets"))
		{
			ImGui::Text("Shadowmap");
			ImGui::Image(depthStencil->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

//	RenderTargetのShaderResourceViewを取得
const SRVHandleList ShadowmapRenderer::GetRenderTargetShaderResourceViews() const
{
	return	SRVHandleList();
}

//	DepthStencilのShaderResourceViewを取得
const SRVHandle ShadowmapRenderer::GetDepthStencilShaderResourceView() const
{
	return	depthStencil->GetShaderResourceView();
}

//	シャドウマップで使用する各行列を算出
void ShadowmapRenderer::CalcShadowmapMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection)
{
	Light* light = LightManager::Instane().GetShadowmapLight();
	if (!light)
	{
		view = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		projection = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		return;
	}

	// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
	DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&light->GetDirection());
	LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);

	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowRect, shadowRect, Near, Far);
	// シャドウマップに描画したい範囲の射影行列を生成
	DirectX::XMStoreFloat4x4(&view, V);
	DirectX::XMStoreFloat4x4(&projection, P);
}

//	描画モデル登録全解除
void ShadowmapRenderer::ClearRenderModel()
{
	model_renderer->ClearRenderModel();
}