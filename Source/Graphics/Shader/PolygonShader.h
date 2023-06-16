#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class PolygonShader : public ModelShader
{
public:
	PolygonShader(ID3D11Device* device);
	~PolygonShader() override {}

	void Begin(const RenderContext& rc) override;
	void SetBuffers(const RenderContext& rc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh) override;
	void DrawSubset(const RenderContext& rc, const ModelResource::Subset& subset) override;
	void End(const RenderContext& rc) override;
private:
	static const int MaxBones = 128;
	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	lightDirection;
		DirectX::XMFLOAT4	viewPosition;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
		DirectX::XMFLOAT4	ka;
		DirectX::XMFLOAT4	kd;
		DirectX::XMFLOAT4	ks;
		float				shiness;
		DirectX::XMFLOAT3	dummy2;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			polygonConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	geometryShader;
	Microsoft::WRL::ComPtr<ID3D11HullShader>		hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		domainShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
};