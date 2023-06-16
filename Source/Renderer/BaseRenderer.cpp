#include "Renderer/BaseRenderer.h"

// �`��^�[�Q�b�g�̑ޔ�
void BaseRenderer::CacheRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSGetViewports(&cachedViewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
}

// �`��^�[�Q�b�g��߂�
void BaseRenderer::RestoreRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(cachedViewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}
