#pragma once

#include "Renderer/BaseRenderer.h"
#include "Renderer/ModelRenderer.h"
#include "Graphics/Gbuffer.h"

#include "Graphics/Shader/Shader3D.h"
#include "Graphics/Shader/PolygonShader.h"
#include "Graphics/Shader/VoronoiShader.h"

#include "Graphics/Shader/ShadowMapShader.h"
#include "Graphics/Shader/SkyboxShader.h"
#include "Graphics/HDRTexture.h"


enum class DEModelRendererId
{
	Default,
	Voronoi,
	Polygon,

	Max,
};

// シーンレンダラー
class DeferredRenderer : public BaseRenderer
{
protected:

public:
	DeferredRenderer(UINT width, UINT height);
	~DeferredRenderer() {}

public:
	//	描画実行
	void Render(ID3D11DeviceContext* dc) override;

	//	デバッグGUI描画
	void DrawDebugGUI() override;

	// 不必要なため空実装
	const SRVHandleList GetRenderTargetShaderResourceViews() const override { return (SRVHandleList)NULL; }
	const SRVHandle GetDepthStencilShaderResourceView() const override { return NULL; }


	//モデルレンダラーゲット
	ModelRenderer* GetModelRenderer(int id) { return model_renderers[id].get(); }

	//	描画モデル登録全解除
	void ClearRenderModel();

	void Initialize();

private:
	//3Dシェーダー
	std::unique_ptr<Shader3D> lambert_shader;
	std::unique_ptr<VoronoiShader> voronoi_shader;
	std::unique_ptr<PolygonShader> polygon_shader;


	std::unique_ptr<Shader3D> depth_shader;

private:
	//2Dシェーダー
	std::unique_ptr<SkyboxShader> skybox_shader;


private:
	//モデルレンダラー
	std::unique_ptr<ModelRenderer> model_renderers[static_cast<int>(DEModelRendererId::Max)];


private:
	bool drawDebugPrimitive = true;		//	デバッグプリミティブ描画

	std::unique_ptr<Gbuffer> gbuffer;

	std::unique_ptr<HDRTexture> environment;
	std::unique_ptr<Sprite> skyboxRenderer;		//	空描画用スプライト

protected:
	// シャドウマップ
	void InitializeShadow();
	void RenderShadow();
	std::unique_ptr<RenderTarget> shadowMap;
	std::unique_ptr<RenderTarget> shadowBlurMap;
	std::unique_ptr<DepthStencil> shadowMapDepth;	// 深度バッファ
	std::unique_ptr<ShadowMapShader> shadowMapShader;
	std::unique_ptr<Shader2D> shadowBlurShader;

};
