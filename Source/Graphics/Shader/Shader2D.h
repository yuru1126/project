#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class Shader2D : public SpriteShader
{
public:
	Shader2D(ID3D11Device* device);
	~Shader2D() override {}

	void Load(const char* PS, bool addblend = false);

	void Begin();
	void End();
	void Draw(const Sprite* sprite);

	// èÉêàâºëzÇÃåpè≥
	void Begin(const RenderContext& rc) { Begin(); }
	void Draw(const RenderContext& rc, const Sprite* sprite) { Draw(sprite); }
	void End(const RenderContext& rc) { End(); }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerStateClamp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerStateBorder;
};
