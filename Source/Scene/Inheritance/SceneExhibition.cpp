#include "Scene/Inheritance/SceneExhibition.h"
#include "Graphics/LightManager.h"
#include "Particle/ParticleManager.h"
#include "Graphics/Model/RegisterModelManager.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Mathf.h"

// 初期化
void SceneExhibition::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	//	各種レンダラー生成
	{
		UINT width = static_cast<UINT>(graphics.GetScreenWidth());
		UINT height = static_cast<UINT>(graphics.GetScreenHeight());

		//	シャドウマップレンダラー
		shadowmapRenderer = std::make_unique<ShadowmapRenderer>(2048);

		//　シーンレンダラー
		sceneRenderer = std::make_unique<DeferredRenderer>(width, height);
	}

	//シェーダーボックス初期化
	{
		shaderBox = std::make_unique<ShaderBox>();
	}

	//カメラ初期設定
	{
		//カメラコントローラー	
		camera_controller = std::make_unique<CameraController>();

		Camera& camera = Camera::Instance();
		camera.SetLookAt(DirectX::XMFLOAT3(0, 10, -10),
			DirectX::XMFLOAT3(0, 0, 0),
			DirectX::XMFLOAT3(0, 1, 0));
		camera.SetPerspectFov(DirectX::XMConvertToRadians(45),
			graphics.GetScreenWidth() / graphics.GetScreenHeight(),
			0.1f,
			2000.0f);
	}

	//パーティクル初期化
	{
		spread_particle* particle = new spread_particle(graphics.GetDevice());
		ParticleManager::Instane().SetParticles(particle);

		spread_particle* fluid_particles = new spread_particle(graphics.GetDevice(), 7000, "ParticleCSUpdateFluid");
		ParticleManager::Instane().SetFluidParticles(fluid_particles);
	}

	// 平行光源を追加
	{
		Light* light = new Light(LightType::Directional);
		light->SetDirection({ 1, -1, -1 });
		// シャドウマップ用の光源として指定する
		LightManager::Instane().SetShadowmapLight(light);
	}

	//パーティクルデータ初期化
	{
		//流体シミュレーション用仮データ(登録はUpdateで行う)
		particle_data.particle_data.position_max = { 2.5f, 5.0f,15.0f,0 };
		particle_data.particle_data.position_min = { -7.5f,-5.0f,15.0f,0 };
		particle_data.particle_data.velocity_max = { 0.0f, 0.0f, 0.0f,1.0f };
		particle_data.particle_data.velocity_min = { 0.0f, 0.0f, 0.0f,1.0f };
		particle_data.particle_data.color_max = { 1,1,1,1 };
		particle_data.particle_data.color_min = { 1,1,1,1 };
		particle_data.particle_data.draw_type = 0;
		particle_data.particle_data.draw_size = 0.65f;
		particle_data.particle_data.set_state = 4;
		particle_data.particle_data.lifespan = { 200.25f ,200.25f };

		fluid_emitter = std::make_unique<Emitter>(particle_data, 1.0f, 1);
		fluid_emitter->SetIntegrateFluid(true);
		fluid_emitter->Stop();



		//花火演出パーティクルセット
		particle_data.particle_data.position_max = { 0,-1.0f,4.0f,0 };
		particle_data.particle_data.position_min = { 0,-1.0f,4.0f,0 };
		particle_data.particle_data.velocity_max = {  0.5f,1.5f, 0.5f,1.0f };
		particle_data.particle_data.velocity_min = { -0.5f,1.5f,-0.5f,1.0f };
		particle_data.particle_data.color_max = { 1,1,0,1 };
		particle_data.particle_data.color_min = { 1,1,0,1 };
		particle_data.particle_data.lifespan = { 2.25f ,2.25f };
		particle_data.particle_data.gravity = 0.01f;
		particle_data.particle_data.draw_type = 0;
		particle_data.particle_data.draw_size = 0.65f;
		particle_data.particle_data.set_state = 0;
		particle_data.sub_emitter_flag = 3;


		particle_data.death_emitter_data = SetDefaultParticleData();

		particle_data.survival_emitter_data = SetDefaultParticleData();
		particle_data.survival_emitter_data.draw_size = 0.7f;
		particle_data.survival_emitter_data.gravity = 0.0f;

		particle_data.survival_emitter_data.lifespan = { 0.25f,0.25f };
		particle_data.survival_emitter_data.set_divide_age = 0.25f;
		particle_data.survival_emitter_data.velocity_normalize_flag = 0;


		particle_data.survival_emitter_data.velocity_max = { 0.0f ,-0.001f, 0.0f,1.0f };
		particle_data.survival_emitter_data.velocity_min = { -0.0f,-0.001f, 0.0f,1.0f };


		emitter = std::make_unique<Emitter>(particle_data, 1.0f,1);
	}
	
	//流体シミュレーション
	{
		fluid_simulation = std::make_unique<FluidSimulation>(graphics.GetDevice());
	}
}


// 終了化
void SceneExhibition::Finalize()
{
	//モデル登録全解除
	shadowmapRenderer->ClearRenderModel();
	sceneRenderer->ClearRenderModel();
	RegisterModelManager::Instance().AllClear();

	//ライト終了化
	LightManager::Instane().Clear();

	//パーティクル終了化
	ParticleManager::Instane().Clear();
}


// 更新処理
void SceneExhibition::Update(float elapsedTime)
{
	spread_particle* particle = ParticleManager::Instane().GetParticles();
	spread_particle* fluid_particle = ParticleManager::Instane().GetFluidParticles();
	//Initializeでするとエラーが出る部分の初期化
	if (is_init)
	{
		is_init = false;
		sceneRenderer->Initialize();
		particle->init(Graphics::Instance().GetDeviceContext());
		fluid_particle->init(Graphics::Instance().GetDeviceContext());

		fluid_simulation->init(Graphics::Instance().GetDeviceContext());
	}

	//カメラコントローラー更新処理
	camera_controller->UpdateOperate(elapsedTime);

	//シェーダーボックス更新処理
	shaderBox->Update(elapsedTime);

	//流体シミュレーション更新
	fluid_simulation->updete(Graphics::Instance().GetDeviceContext(), elapsedTime);

	//エミッター更新処理
	emitter->_SetParticleData(particle_data);
	emitter->Update(elapsedTime);
	
	particle->updete(Graphics::Instance().GetDeviceContext(), elapsedTime);
	
	//パーティクル更新処理
	fluid_simulation->Begin(Graphics::Instance().GetDeviceContext());
	fluid_particle->updete(Graphics::Instance().GetDeviceContext(), elapsedTime);
	fluid_simulation->End(Graphics::Instance().GetDeviceContext());
	

	//追加モデル登録,登録モデル解除
	RegisterRenderModel();
}


// 描画処理
void SceneExhibition::Render()
{
	spread_particle* particle = ParticleManager::Instane().GetParticles();
	spread_particle* fluid_particle = ParticleManager::Instane().GetFluidParticles();

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// 画面クリア＆レンダーターゲット設定
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// シャドウマップの描画
	shadowmapRenderer->Render(dc);

	// シーンの描画
	sceneRenderer->Render(dc);

	RenderContext rc{};
	{
		rc.view = camera.GetView();
		rc.projection = camera.GetProjection();
	}

	// デバッグ情報の表示
	{
		ImGui::Begin("ImGUI", nullptr, ImGuiWindowFlags_None);
		
		if (is_particle)
		{
			{
				ImGui::SliderFloat3("position", &particle_data.particle_data.position_max.x, -10.0, 10.0);
				particle_data.particle_data.position_min = particle_data.particle_data.position_max;

				ImGui::SliderFloat3("velocity_max", &particle_data.particle_data.velocity_max.x, -5.0, 5.0);
				ImGui::SliderFloat3("velocity_min", &particle_data.particle_data.velocity_min.x, -5.0, 5.0);

				bool normalize_flag = particle_data.particle_data.velocity_normalize_flag;
				if (ImGui::Checkbox("velocity_normalize_flag", &normalize_flag))
				{
					particle_data.particle_data.velocity_normalize_flag = normalize_flag;
				}
				if (normalize_flag)
				{
					ImGui::SliderFloat("velocity_length_max", &particle_data.particle_data.velocity_max.w, 0.0, 5.0);
					ImGui::SliderFloat("velocity_length_min", &particle_data.particle_data.velocity_min.w, 0.0, 5.0);
				}

				ImGui::SliderFloat("lifespan_max", &particle_data.particle_data.lifespan.x, 0.0, 5.0);
				ImGui::SliderFloat("lifespan_min", &particle_data.particle_data.lifespan.y, 0.0, 5.0);
				ImGui::SliderFloat("gravity", &particle_data.particle_data.gravity, 0.0, 1.0);
				ImGui::SliderFloat("draw_size", &particle_data.particle_data.draw_size, 0.0, 2.0);

				float interval = emitter->GetInterval();
				ImGui::SliderFloat("interval", &interval, 0.017f, 0.75f);
				emitter->SetInterval(interval);

				int count = emitter->GetCount();
				ImGui::InputInt("count", &count);
				if (ImGui::Button("count +50")) { count += 50; }
				if (ImGui::Button("count -50")) { count -= 50; }
				count = (std::max)(0, count);
				emitter->SetCount(count);

				ImGui::InputInt("sub_emitter_flag", &particle_data.sub_emitter_flag);
				if (particle_data.sub_emitter_flag < 0) { particle_data.sub_emitter_flag = 0; }
				if (particle_data.sub_emitter_flag > 3) { particle_data.sub_emitter_flag = 3; }

				ParticleManager::Instane().GetParticles()->DrawDebugGUI();
			}
		}
		else if (is_polygon)
		{
			shaderBox->DrawDebugGUI();
		}



		if (ImGui::Button("Firework"))
		{
			is_particle = true;
			is_polygon = false;
			is_fluid = false;

			shaderBox->UnRegister();
			emitter->Start();

			camera_controller->Reset();

			fluid_particle->init(Graphics::Instance().GetDeviceContext());
		}

		if (ImGui::Button("Annihilation_Timer"))
		{
			is_particle = false;
			is_polygon = true;
			is_fluid = false;

			particle->init(Graphics::Instance().GetDeviceContext());
			fluid_particle->init(Graphics::Instance().GetDeviceContext());
			emitter->Stop();
			shaderBox->Register();

			shaderBox->SetGeometryType(Geometr_type::DEATH);
			camera_controller->Reset();
		}
		if (ImGui::Button("Annihilation"))
		{
			is_particle = false;
			is_polygon = true;
			is_fluid = false;
			
			particle->init(Graphics::Instance().GetDeviceContext());
			fluid_particle->init(Graphics::Instance().GetDeviceContext());
			emitter->Stop();
			shaderBox->Register();

			shaderBox->SetGeometryType(Geometr_type::DEATH_NOT_TIMER);
			camera_controller->Reset();
		}
		if (ImGui::Button("Birth_Timer"))
		{
			is_particle = false;
			is_polygon = true;
			is_fluid = false;

			particle->init(Graphics::Instance().GetDeviceContext());
			fluid_particle->init(Graphics::Instance().GetDeviceContext());
			emitter->Stop();
			shaderBox->Register();

			shaderBox->SetGeometryType(Geometr_type::BIRTH);
			camera_controller->Reset();
		}
		if (ImGui::Button("Birth"))
		{
			is_particle = false;
			is_polygon = true;
			is_fluid = false;

			particle->init(Graphics::Instance().GetDeviceContext());
			fluid_particle->init(Graphics::Instance().GetDeviceContext());
			emitter->Stop();
			shaderBox->Register();

			shaderBox->SetGeometryType(Geometr_type::BIRTH_NOT_TIMER);
			camera_controller->Reset();
		}
		if (ImGui::Button("FluidSimulation"))
		{
			fluid_particle->init(Graphics::Instance().GetDeviceContext());
			fluid_simulation->init(Graphics::Instance().GetDeviceContext());
			for (int i = 0; i < 5; i++)
			{
				fluid_emitter->Integrate(1000);
			}

			is_particle = false;
			is_polygon = false;
			is_fluid = true;

			shaderBox->UnRegister();
			particle->init(Graphics::Instance().GetDeviceContext());
			emitter->Stop();
			camera_controller->Reset();
		}

		ImGui::End();
	}
}


//追加モデル登録,登録モデル解除
void SceneExhibition::RegisterRenderModel()
{
	RegisterModelManager& rm = RegisterModelManager::Instance();
	ModelRenderer* model_renderer = shadowmapRenderer->GetModelRenderer();
	rm.AllRegisterRenderModel(model_renderer);
	rm.AllUnregisterRenderModel(model_renderer);

	model_renderer = sceneRenderer->GetModelRenderer(static_cast<int>(DEModelRendererId::Default));
	rm.RegisterRenderModel(model_renderer);
	rm.UnregisterRenderModel(model_renderer);

	model_renderer = sceneRenderer->GetModelRenderer(static_cast<int>(DEModelRendererId::Voronoi));
	rm.VoronoiRegisterRenderModel(model_renderer);
	rm.VoronoiUnregisterRenderModel(model_renderer);

	model_renderer = sceneRenderer->GetModelRenderer(static_cast<int>(DEModelRendererId::Polygon));
	rm.PolygonRegisterRenderModel(model_renderer);
	rm.PolygonUnregisterRenderModel(model_renderer);

	rm.AllClear();
}