#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/RenderContext.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/Texture.h"

using	SRVHandle = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;
using	SRVHandleList = std::vector<SRVHandle>;

class BaseRenderer
{
public:
	BaseRenderer() {}
	virtual ~BaseRenderer() {}

	// 描画実行
	virtual void Render(ID3D11DeviceContext* dc) = 0;

	// デバッグ描画
	virtual void DrawDebugGUI() = 0;

	// RenderTargetのShaderResourceViewを取得
	virtual const SRVHandleList GetRenderTargetShaderResourceViews() const = 0;

	// DepthStencilのShaderResourceViewを取得
	virtual const SRVHandle GetDepthStencilShaderResourceView() const = 0;

protected:
	// 描画ターゲットの退避
	void CacheRenderTargets(ID3D11DeviceContext* dc);

	// 描画ターゲットを戻す
	void RestoreRenderTargets(ID3D11DeviceContext* dc);

private:
	UINT			cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView;
};
