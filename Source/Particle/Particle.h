#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <functional>
#include "Graphics/Shader.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Texture.h"


class spread_particle
{
public:
	//構造体宣言
	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	viewPosition;
	};

	struct particle_constants
	{
		int particle_count{};
		float particle_size{ 0.1f };
		float elapsed_time{};
		float progress_time = 0.0f;

		DirectX::XMFLOAT4 setHSV = { 0,0.75f,0.85f,0 };

		float divide_length = 40.0f;
		int count = 0;


		DirectX::XMFLOAT2 dummy{};
		//float dummy{};
	};

	//パーティクルデータ
	struct ParticleData
	{
		DirectX::XMFLOAT4 position_max{};
		DirectX::XMFLOAT4 position_min{};

		//速度 : Wにnormalize時の速度を入れておく
		DirectX::XMFLOAT4 velocity_max = { 0,0,0,1 };
		DirectX::XMFLOAT4 velocity_min = { 0,0,0,1 };

		DirectX::XMFLOAT4 color_max{};
		DirectX::XMFLOAT4 color_min{};

		DirectX::XMFLOAT2 lifespan = { 0.0f ,0.0f };		//寿命
		float set_divide_age = 0.0f;//寿命によるサイズ縮小

		float gravity = 0.0f;		//重力影響度
		float draw_size = 1;		//描画サイズ

		float animation_speed = 0.0f;
		float length_shrink_flag = 1.0f;//距離によるサイズ縮小

		float appearance_frequency = 0.0165f;//サブエミッターの出現頻度

		//float dummy = 0;
		DirectX::XMFLOAT4 dummy{};



		int draw_type = 0;			//描画タイプ
		DirectX::XMINT3 tiles = { 1,1,1 };
		DirectX::XMINT2 set_tiles = { 0,0 };
		int set_state = 0;
		int hsv_data_flag = 0; //HSVデータかの判定
		int velocity_normalize_flag = 0;//velocityのノーマル化判定

		//int dummyI = 0;
		DirectX::XMINT3 dummyI{};
	};

	struct SetParticleData
	{
		ParticleData particle_data;
		ParticleData death_emitter_data;
		ParticleData survival_emitter_data;

		//サブエミッター設定
		int sub_emitter_flag = 0;

		DirectX::XMFLOAT3 dummy{};
	};

	//パーティクルのステータス
	struct particle
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 velocity;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT4 color;
		float gravity;						//重力影響度
		float age;

		DirectX::XMFLOAT2 length_shrink;	//x:距離によるサイズ縮小 y:使用フラグ
		float size;							//描画サイズ
		float divide_age;					//寿命によるサイズ縮小 0～1でクランプ

		float use_tile;						//使用するタイル
		float animation_speed;				//アニメーション時の速度

		float appearance_timer;				//サブエミッターの出現タイマー
		float appearance_frequency;			//出現頻度



		//float dummy;
		DirectX::XMFLOAT2 dummy;


		int draw_type;						//描画タイプ
		DirectX::XMINT3 tiles;				//x:X方向のタイル数  y:方向のタイル数  z:最大タイル数
		int state;
		//サブエミッター設定
		int sub_emitter_flag;

		//int dummyI = 0;
		DirectX::XMINT2 dummyI{};
	};


public:
	spread_particle(ID3D11Device* device, size_t max_particle_count = 1000000);
	spread_particle(ID3D11Device* device, size_t max_particle_count, const char* CSUpdete);

	//spread_particle(ID3D11Device* device, size_t max_particle_count, const char* CSUpdete);

	void input(ID3D11DeviceContext* dc, const RenderContext& rc);

	//初期化
	void init(ID3D11DeviceContext* dc);
	//更新処理
	void updete(ID3D11DeviceContext* dc, float elapsedTime);
	//描画処理
	void render(ID3D11DeviceContext* dc);

	//登録処理
	void integrate(ID3D11DeviceContext* dc, SetParticleData data, int count);


	//ゲット、セット
	int GetNowCount() { return static_cast<int>(now_count); }
	int GetUseCount() { return static_cast<int>(use_count); }


	//デバック用GUI描画
	void DrawDebugGUI();

private:
	//描画バッファへ登録
	void Register(ID3D11DeviceContext* dc);

private:

	particle_constants particle_data;
	size_t max_particle_count;

	//未使用の数
	UINT now_count = 0;

	//現在使われているカウント数
	UINT use_count = 0;

	//登録数
	UINT regist_count = 0;


	//描画、更新用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_append_buffer_uav;

	//登録用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> register_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> register_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> register_particle_append_buffer_uav;

	//カウント用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> now_count_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> now_count_particle_counter_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> now_count_particle_append_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_count_buffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_buffer_srv;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> init_compute_shader;		//描画用バッファの初期化
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> set_compute_shader;			//登録用バッファに値を入れる
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> register_compute_shader;	//描画用バッファに登録
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_compute_shader;		//描画用バッファの更新
	
	//サブエミッター用コンピュートシェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> set_one_compute_shader;			//登録用バッファに値を入れる
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> register_one_compute_shader;	//描画用バッファに登録


	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_constant_buffer;


	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;

	//テクスチャ
	std::unique_ptr<Texture> texture[3];
};


spread_particle::ParticleData SetDefaultParticleData();