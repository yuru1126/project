#pragma once

#include "Renderer/BaseRenderer.h"
#include "Renderer/ModelRenderer.h"
#include "Graphics/Gbuffer.h"

#include "Graphics/Shader/Shader3D.h"
#include "Graphics/Shader/PolygonShader.h"
#include "Graphics/Shader/VoronoiShader.h"

#include "Graphics/Shader/ShadowMapShader.h"
#include "Graphics/Shader/SkyboxShader.h"
#include "Graphics/HDRTexture.h"


enum class DEModelRendererId
{
	Default,
	Voronoi,
	Polygon,

	Max,
};

// �V�[�������_���[
class DeferredRenderer : public BaseRenderer
{
protected:

public:
	DeferredRenderer(UINT width, UINT height);
	~DeferredRenderer() {}

public:
	//	�`����s
	void Render(ID3D11DeviceContext* dc) override;

	//	�f�o�b�OGUI�`��
	void DrawDebugGUI() override;

	// �s�K�v�Ȃ��ߋ����
	const SRVHandleList GetRenderTargetShaderResourceViews() const override { return (SRVHandleList)NULL; }
	const SRVHandle GetDepthStencilShaderResourceView() const override { return NULL; }


	//���f�������_���[�Q�b�g
	ModelRenderer* GetModelRenderer(int id) { return model_renderers[id].get(); }

	//	�`�惂�f���o�^�S����
	void ClearRenderModel();

	void Initialize();

private:
	//3D�V�F�[�_�[
	std::unique_ptr<Shader3D> lambert_shader;
	std::unique_ptr<VoronoiShader> voronoi_shader;
	std::unique_ptr<PolygonShader> polygon_shader;


	std::unique_ptr<Shader3D> depth_shader;

private:
	//2D�V�F�[�_�[
	std::unique_ptr<SkyboxShader> skybox_shader;


private:
	//���f�������_���[
	std::unique_ptr<ModelRenderer> model_renderers[static_cast<int>(DEModelRendererId::Max)];


private:
	bool drawDebugPrimitive = true;		//	�f�o�b�O�v���~�e�B�u�`��

	std::unique_ptr<Gbuffer> gbuffer;

	std::unique_ptr<HDRTexture> environment;
	std::unique_ptr<Sprite> skyboxRenderer;		//	��`��p�X�v���C�g

protected:
	// �V���h�E�}�b�v
	void InitializeShadow();
	void RenderShadow();
	std::unique_ptr<RenderTarget> shadowMap;
	std::unique_ptr<RenderTarget> shadowBlurMap;
	std::unique_ptr<DepthStencil> shadowMapDepth;	// �[�x�o�b�t�@
	std::unique_ptr<ShadowMapShader> shadowMapShader;
	std::unique_ptr<Shader2D> shadowBlurShader;

};
