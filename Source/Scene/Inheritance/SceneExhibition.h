#pragma once
#include "Scene/Scene.h"
#include "ShaderBox.h"
#include "CameraController.h"
#include "Renderer/DeferredRenderer.h"
#include "Renderer/ShadowmapRenderer.h"
#include "Graphics/Sprite.h"
#include "Particle/Particle.h"
#include "Graphics/RenderContext.h"
#include "Particle/Emitter.h"
#include "FluidSimulation/FluidSimulation.h"


class SceneExhibition : public Scene
{
public:
	SceneExhibition() {}
	~SceneExhibition() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;
private:
	//�ǉ����f���o�^,�o�^���f������
	void RegisterRenderModel();
private:
	bool is_init = true;

	DirectX::XMFLOAT3 target = { 0.0f,0.5f,0.0f };


	std::unique_ptr<CameraController> camera_controller;

	//�V�F�[�_�[�{�b�N�X
	std::unique_ptr<ShaderBox> shaderBox;

	//�G�~�b�^�[
	std::unique_ptr<Emitter>		emitter;

	//���̃V�~�����[�V�����p�G�~�b�^�[
	std::unique_ptr<Emitter>		fluid_emitter;

	//�p�[�e�B�N���f�[�^
	spread_particle::SetParticleData particle_data;

	//���̃V�~�����[�V����
	std::unique_ptr<FluidSimulation> fluid_simulation;

	std::unique_ptr<DeferredRenderer>		sceneRenderer;
	std::unique_ptr<ShadowmapRenderer>		shadowmapRenderer;


	//�f�o�b�OGUI�֘A
	bool is_particle = true;
	bool is_polygon = false;
	bool is_fluid = false;

};