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
	//	�`����s
	void Render(ID3D11DeviceContext* dc) override;

	//	�f�o�b�OGUI�`��
	void DrawDebugGUI() override;

	//	RenderTarget��ShaderResourceView���擾
	const SRVHandleList GetRenderTargetShaderResourceViews() const override;

	//	DepthStencil��ShaderResourceView���擾
	const SRVHandle GetDepthStencilShaderResourceView() const override;

	//���f�������_���[�Q�b�g
	ModelRenderer* GetModelRenderer() { return model_renderer.get(); }

	//	�`�惂�f���o�^�S����
	void ClearRenderModel();

private:
	//	�V���h�E�}�b�v�Ŏg�p����e�s����Z�o
	void CalcShadowmapMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection);

private:
	//���f�������_���[
	std::unique_ptr<ModelRenderer> model_renderer;

	//3D�V�F�[�_�[
	std::unique_ptr<ShadowmapCasterShader> shadowmap_caster_shader;

private:
	std::unique_ptr<DepthStencil>	depthStencil;						// �V���h�E�}�b�v�p�[�x�o�b�t�@
	float							shadowRect = 500.0f;				// �`��͈�
	float							shadowBias = 0.0001f;				// �[�x��r�p�̃I�t�Z�b�g�l
	DirectX::XMFLOAT3				shadowColor = { 0.2f, 0.2f, 0.2f };	// �e�̐F
};