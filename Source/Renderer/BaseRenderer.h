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

	// �`����s
	virtual void Render(ID3D11DeviceContext* dc) = 0;

	// �f�o�b�O�`��
	virtual void DrawDebugGUI() = 0;

	// RenderTarget��ShaderResourceView���擾
	virtual const SRVHandleList GetRenderTargetShaderResourceViews() const = 0;

	// DepthStencil��ShaderResourceView���擾
	virtual const SRVHandle GetDepthStencilShaderResourceView() const = 0;

protected:
	// �`��^�[�Q�b�g�̑ޔ�
	void CacheRenderTargets(ID3D11DeviceContext* dc);

	// �`��^�[�Q�b�g��߂�
	void RestoreRenderTargets(ID3D11DeviceContext* dc);

private:
	UINT			cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView;
};
