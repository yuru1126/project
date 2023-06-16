#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class ProjectionTextureMapShader : public SpriteShader
{
public:
	ProjectionTextureMapShader(ID3D11Device* device);
	~ProjectionTextureMapShader() override {}

	void End();

	// èÉêàâºëzÇÃåpè≥
	void Begin(const RenderContext& rc);
	void Draw(const RenderContext& rc, const Sprite* sprite);
	void End(const RenderContext& rc) { End(); }

private:
	struct CbScene
	{
		DirectX::XMFLOAT4X4	inverseViewProjection;
		DirectX::XMFLOAT4X4	viewProjection;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			textureMapConstantBuffer;


	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState2;
};