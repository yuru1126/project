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

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;
private:
	//追加モデル登録,登録モデル解除
	void RegisterRenderModel();
private:
	bool is_init = true;

	DirectX::XMFLOAT3 target = { 0.0f,0.5f,0.0f };


	std::unique_ptr<CameraController> camera_controller;

	//シェーダーボックス
	std::unique_ptr<ShaderBox> shaderBox;

	//エミッター
	std::unique_ptr<Emitter>		emitter;

	//流体シミュレーション用エミッター
	std::unique_ptr<Emitter>		fluid_emitter;

	//パーティクルデータ
	spread_particle::SetParticleData particle_data;

	//流体シミュレーション
	std::unique_ptr<FluidSimulation> fluid_simulation;

	std::unique_ptr<DeferredRenderer>		sceneRenderer;
	std::unique_ptr<ShadowmapRenderer>		shadowmapRenderer;


	//デバッグGUI関連
	bool is_particle = true;
	bool is_polygon = false;
	bool is_fluid = false;

};