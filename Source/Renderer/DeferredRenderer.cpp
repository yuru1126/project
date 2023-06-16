#include "DeferredRenderer.h"

#include "Graphics/LightManager.h"
#include "Camera.h"
#include "Particle/ParticleManager.h"

DeferredRenderer::DeferredRenderer(UINT width, UINT height)
{
	//���f�������_���[������
	for (int i = 0; i < static_cast<int>(DEModelRendererId::Max); i++)
	{
		model_renderers[i] = std::make_unique<ModelRenderer>();
	}

	//3D�V�F�[�_�[������
	voronoi_shader = std::make_unique<VoronoiShader>(Graphics::Instance().GetDevice());
	polygon_shader = std::make_unique<PolygonShader>(Graphics::Instance().GetDevice());
	lambert_shader = std::make_unique<Shader3D>(Graphics::Instance().GetDevice());
	lambert_shader->Load("LambertVS", "LambertPS");

	depth_shader = std::make_unique<Shader3D>(Graphics::Instance().GetDevice());
	depth_shader->Load("DepthShaderVS", "DepthShaderPS");

	// GBuffer������
	gbuffer = std::make_unique<Gbuffer>();
	gbuffer->Initialize();

	// ��`��p�X�v���C�g
	skyboxRenderer = std::make_unique<Sprite>();
	skybox_shader = std::make_unique<SkyboxShader>(Graphics::Instance().GetDevice());

	// �V���h�E�}�b�v������
	InitializeShadow();

	environment = std::make_unique<HDRTexture>();
	environment->Load("Data/Environment/clarens_midday_4k.hdr");

	return;
}

void DeferredRenderer::Initialize()
{
	environment->SetDesc();
	environment->Set(15);
}

void DeferredRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	// ���ݐݒ肳��Ă���o�b�t�@��ޔ�
	CacheRenderTargets(dc);

	// �V���h�E�}�b�v�`��
	RenderShadow();

	// GBuffer�Z�b�g
	gbuffer->Activate();

	// �`��R���e�L�X�g�ɏ����Z�b�g
	RenderContext rc{};
	{
		rc.deviceContext = dc;
		rc.screenSize.x = graphics.GetScreenWidth();
		rc.screenSize.y = graphics.GetScreenHeight();
		rc.screenSize.z = camera.GetNearZ();
		rc.screenSize.w = camera.GetFarZ();

		// �J�����p�����[�^�ݒ�
		rc.viewPosition.x = camera.GetEye().x;
		rc.viewPosition.y = camera.GetEye().y;
		rc.viewPosition.z = camera.GetEye().z;
		rc.viewPosition.w = 1;
		rc.view = camera.GetView();
		rc.projection = camera.GetProjection();
	}

	{
		// ��e�N�X�`����ݒ�
		skyboxRenderer->SetShaderResourceView(environment->GetShaderResourceView().Get(),
			environment->GetWidth(),
			environment->GetHeight());
		skyboxRenderer->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
			0, 0, static_cast<float>(environment->GetWidth()), static_cast<float>(environment->GetHeight()),
			0, 1, 1, 1, 1);
		// ��`��
		skybox_shader->Begin(rc);
		skybox_shader->Draw(rc, skyboxRenderer.get());
		skybox_shader->End(rc);
	}

	//�����o�[�g�`��
	lambert_shader->Begin(rc);
	ModelRenderer* model_renderer = model_renderers[static_cast<int>(DEModelRendererId::Default)].get();
	int model_count = model_renderer->GetModelCount();
	for (int i = 0; i < model_count; i++)
	{
		Model* model = model_renderer->GetRenderModel(i);

		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();
		
		model->SetRenderContext(&rc);
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	���_�E�C���f�b�N�X�o�b�t�@���ݒ�
			lambert_shader->SetBuffers(rc, nodes, mesh);
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	�T�u�Z�b�g�P�ʂŕ`��
				lambert_shader->DrawSubset(rc, subset);
			}
		}
	}
	lambert_shader->End(rc);

	//�{���m�C�`��
	voronoi_shader->Begin(rc);
	model_renderer = model_renderers[static_cast<int>(DEModelRendererId::Voronoi)].get();
	model_count = model_renderer->GetModelCount();
	for (int i = 0; i < model_count; i++)
	{
		Model* model = model_renderer->GetRenderModel(i);

		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();

		model->SetRenderContext(&rc);
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	���_�E�C���f�b�N�X�o�b�t�@���ݒ�
			voronoi_shader->SetBuffers(rc, nodes, mesh);
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	�T�u�Z�b�g�P�ʂŕ`��
				voronoi_shader->DrawSubset(rc, subset);
			}
		}
	}
	voronoi_shader->End(rc);

	//�|���S���`��
	polygon_shader->Begin(rc);
	model_renderer = model_renderers[static_cast<int>(DEModelRendererId::Polygon)].get();
	model_count = model_renderer->GetModelCount();
	for (int i = 0; i < model_count; i++)
	{
		Model* model = model_renderer->GetRenderModel(i);

		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();

		model->SetRenderContext(&rc);
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			//	���_�E�C���f�b�N�X�o�b�t�@���ݒ�
			polygon_shader->SetBuffers(rc, nodes, mesh);
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	�T�u�Z�b�g�P�ʂŕ`��
				polygon_shader->DrawSubset(rc, subset);
			}
		}
	}
	polygon_shader->End(rc);


	//�p�[�e�B�N���`��
	{
		spread_particle* particle = ParticleManager::Instane().GetParticles();
		particle->input(dc, rc);
		particle->render(dc);

		//���̃V�~�����[�V�����p�p�[�e�B�N��
		spread_particle* fluid_particle = ParticleManager::Instane().GetFluidParticles();
		fluid_particle->input(dc, rc);
		fluid_particle->render(dc);
	}

	//�f�o�b�O�v���~�e�B�u�̕\��
	if (drawDebugPrimitive)
	{

		// ���C�g�̃f�o�b�O�v���~�e�B�u�`��
		LightManager::Instane().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(rc.deviceContext, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(rc.deviceContext, rc.view, rc.projection);

	}

	// GBuffer�Z�b�g
	gbuffer->Deactivate();

	//	���̃o�b�t�@�ɖ߂�
	RestoreRenderTargets(dc);

	gbuffer->Render();

	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}


void DeferredRenderer::InitializeShadow()
{
	int ShadowMapSize = 1024;
	// �J���[�o�b�t�@�쐬
	shadowMap = std::make_unique<RenderTarget>(ShadowMapSize, ShadowMapSize, DXGI_FORMAT_R16G16_FLOAT);
	shadowBlurMap = std::make_unique<RenderTarget>(ShadowMapSize, ShadowMapSize, DXGI_FORMAT_R16G16_FLOAT);
	// �[�x�o�b�t�@����
	shadowMapDepth = std::make_unique<DepthStencil>(ShadowMapSize, ShadowMapSize);

	shadowMapShader = std::make_unique<ShadowMapShader>(Graphics::Instance().GetDevice());
	shadowBlurShader = std::make_unique<Shader2D>(Graphics::Instance().GetDevice());
	shadowBlurShader->Load("ShadowMapBlurPS");

}

void DeferredRenderer::RenderShadow()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderContext rc{};
	{
		rc.deviceContext = dc;
	}

	// ���ݐݒ肳��Ă���o�b�t�@��ޔ�
	CacheRenderTargets(dc);

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	ID3D11DepthStencilView* dsv = shadowMapDepth->GetDepthStencilView().Get();
	float clearColor[] = { FLT_MAX, FLT_MAX, FLT_MAX, 1.0f };
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->ClearRenderTargetView(shadowMap->GetRenderTargetView().Get(), clearColor);

	ID3D11ShaderResourceView* srvs[1] = { NULL };
	rc.deviceContext->PSSetShaderResources(14, 1, srvs);
	dc->OMSetRenderTargets(1, shadowMap->GetRenderTargetView().GetAddressOf(), dsv);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(shadowMap->GetWidth());
	vp.Height = static_cast<float>(shadowMap->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// �o�^���f����`��
	shadowMapShader->Begin(rc);
	for (int i = 0; i < static_cast<int>(DEModelRendererId::Max); i++)
	{
		ModelRenderer* model_renderer = model_renderers[i].get();
		for (const Model* model : model_renderer->GetRenderModels())
		{
			const ModelResource* resource = model->GetResource();
			const std::vector<Model::Node>& nodes = model->GetNodes();
			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				//	���_�E�C���f�b�N�X�o�b�t�@���ݒ�
				shadowMapShader->SetBuffers(rc, nodes, mesh);

				//	�T�u�Z�b�g�P�ʂŕ`��
				for (const ModelResource::Subset& subset : mesh.subsets)
				{
					shadowMapShader->DrawSubset(rc, subset);
				}
			}
		}
	}
	shadowMapShader->End(rc);

	// �ڂ���
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, shadowBlurMap->GetRenderTargetView().GetAddressOf(), dsv);
	vp.Width = static_cast<float>(shadowMap->GetWidth());
	vp.Height = static_cast<float>(shadowMap->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	{
		skyboxRenderer->SetShaderResourceView(shadowMap->GetShaderResourceView().Get(),
			shadowMap->GetWidth(),
			shadowMap->GetHeight());
		skyboxRenderer->Update(
			0, 0, static_cast<float>(shadowMap->GetWidth()), static_cast<float>(shadowMap->GetHeight()),
			0, 0, static_cast<float>(shadowMap->GetWidth()), static_cast<float>(shadowMap->GetHeight()),
			0, 1, 1, 1, 1);
		// ��`��
		shadowBlurShader->Begin(rc);
		shadowBlurShader->Draw(rc, skyboxRenderer.get());
		shadowBlurShader->End(rc);
	}

	//	���̃o�b�t�@�ɖ߂�
	RestoreRenderTargets(dc);

	rc.deviceContext->PSSetShaderResources(14, 1, shadowMap->GetShaderResourceView().GetAddressOf());

}


//	�f�o�b�OGUI�`��
void DeferredRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("Scene Rendering"))
	{
		ImGui::Checkbox("DebugPrimitive", &drawDebugPrimitive);

		ImGui::Text("ShadowMap");
		ImGui::Image(shadowBlurMap->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

		ImGui::TreePop();
	}
	gbuffer->DrawDebugGUI();

}

//	�`�惂�f���o�^�S����
void DeferredRenderer::ClearRenderModel()
{
	for (int i = 0; i < static_cast<int>(DEModelRendererId::Max); i++)
	{
		model_renderers[i]->ClearRenderModel();
	}
}