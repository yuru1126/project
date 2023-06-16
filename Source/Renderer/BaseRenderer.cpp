#include "Renderer/BaseRenderer.h"

// •`‰æƒ^[ƒQƒbƒg‚Ì‘Þ”ð
void BaseRenderer::CacheRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSGetViewports(&cachedViewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
}

// •`‰æƒ^[ƒQƒbƒg‚ð–ß‚·
void BaseRenderer::RestoreRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(cachedViewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}
