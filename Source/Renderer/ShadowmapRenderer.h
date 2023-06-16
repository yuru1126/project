#pragma once
#include "Renderer/BaseRenderer.h"
#include "Renderer/ModelRenderer.h"
#include "Graphics/Shader/ShadowmapCasterShader.h"

class ShadowmapRenderer : public BaseRenderer
{
	static constexpr float Near = 1.0f;
	static constexpr float Far = 10000.0f;
public:
	ShadowmapRenderer(UINT shadowmapSize);
	~ShadowmapRenderer();


public:
	//	描画実行
	void Render(ID3D11DeviceContext* dc) override;

	//	デバッグGUI描画
	void DrawDebugGUI() override;

	//	RenderTargetのShaderResourceViewを取得
	const SRVHandleList GetRenderTargetShaderResourceViews() const override;

	//	DepthStencilのShaderResourceViewを取得
	const SRVHandle GetDepthStencilShaderResourceView() const override;

	//モデルレンダラーゲット
	ModelRenderer* GetModelRenderer() { return model_renderer.get(); }

	//	描画モデル登録全解除
	void ClearRenderModel();

private:
	//	シャドウマップで使用する各行列を算出
	void CalcShadowmapMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection);

private:
	//モデルレンダラー
	std::unique_ptr<ModelRenderer> model_renderer;

	//3Dシェーダー
	std::unique_ptr<ShadowmapCasterShader> shadowmap_caster_shader;

private:
	std::unique_ptr<DepthStencil>	depthStencil;						// シャドウマップ用深度バッファ
	float							shadowRect = 500.0f;				// 描画範囲
	float							shadowBias = 0.0001f;				// 深度比較用のオフセット値
	DirectX::XMFLOAT3				shadowColor = { 0.2f, 0.2f, 0.2f };	// 影の色
};