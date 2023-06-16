#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class ShadowmapCasterShader : public ModelShader
{
public:
	ShadowmapCasterShader(ID3D11Device* device);
	~ShadowmapCasterShader() override {}

	void Begin(const RenderContext& rc) override;
	void SetBuffers(const RenderContext& rc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh) override;
	void DrawSubset(const RenderContext& rc, const ModelResource::Subset& subset) override;
	void End(const RenderContext& rc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;
};
