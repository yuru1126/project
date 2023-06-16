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
	//�\���̐錾
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

	//�p�[�e�B�N���f�[�^
	struct ParticleData
	{
		DirectX::XMFLOAT4 position_max{};
		DirectX::XMFLOAT4 position_min{};

		//���x : W��normalize���̑��x�����Ă���
		DirectX::XMFLOAT4 velocity_max = { 0,0,0,1 };
		DirectX::XMFLOAT4 velocity_min = { 0,0,0,1 };

		DirectX::XMFLOAT4 color_max{};
		DirectX::XMFLOAT4 color_min{};

		DirectX::XMFLOAT2 lifespan = { 0.0f ,0.0f };		//����
		float set_divide_age = 0.0f;//�����ɂ��T�C�Y�k��

		float gravity = 0.0f;		//�d�͉e���x
		float draw_size = 1;		//�`��T�C�Y

		float animation_speed = 0.0f;
		float length_shrink_flag = 1.0f;//�����ɂ��T�C�Y�k��

		float appearance_frequency = 0.0165f;//�T�u�G�~�b�^�[�̏o���p�x

		//float dummy = 0;
		DirectX::XMFLOAT4 dummy{};



		int draw_type = 0;			//�`��^�C�v
		DirectX::XMINT3 tiles = { 1,1,1 };
		DirectX::XMINT2 set_tiles = { 0,0 };
		int set_state = 0;
		int hsv_data_flag = 0; //HSV�f�[�^���̔���
		int velocity_normalize_flag = 0;//velocity�̃m�[�}��������

		//int dummyI = 0;
		DirectX::XMINT3 dummyI{};
	};

	struct SetParticleData
	{
		ParticleData particle_data;
		ParticleData death_emitter_data;
		ParticleData survival_emitter_data;

		//�T�u�G�~�b�^�[�ݒ�
		int sub_emitter_flag = 0;

		DirectX::XMFLOAT3 dummy{};
	};

	//�p�[�e�B�N���̃X�e�[�^�X
	struct particle
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 velocity;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT4 color;
		float gravity;						//�d�͉e���x
		float age;

		DirectX::XMFLOAT2 length_shrink;	//x:�����ɂ��T�C�Y�k�� y:�g�p�t���O
		float size;							//�`��T�C�Y
		float divide_age;					//�����ɂ��T�C�Y�k�� 0�`1�ŃN�����v

		float use_tile;						//�g�p����^�C��
		float animation_speed;				//�A�j���[�V�������̑��x

		float appearance_timer;				//�T�u�G�~�b�^�[�̏o���^�C�}�[
		float appearance_frequency;			//�o���p�x



		//float dummy;
		DirectX::XMFLOAT2 dummy;


		int draw_type;						//�`��^�C�v
		DirectX::XMINT3 tiles;				//x:X�����̃^�C����  y:�����̃^�C����  z:�ő�^�C����
		int state;
		//�T�u�G�~�b�^�[�ݒ�
		int sub_emitter_flag;

		//int dummyI = 0;
		DirectX::XMINT2 dummyI{};
	};


public:
	spread_particle(ID3D11Device* device, size_t max_particle_count = 1000000);
	spread_particle(ID3D11Device* device, size_t max_particle_count, const char* CSUpdete);

	//spread_particle(ID3D11Device* device, size_t max_particle_count, const char* CSUpdete);

	void input(ID3D11DeviceContext* dc, const RenderContext& rc);

	//������
	void init(ID3D11DeviceContext* dc);
	//�X�V����
	void updete(ID3D11DeviceContext* dc, float elapsedTime);
	//�`�揈��
	void render(ID3D11DeviceContext* dc);

	//�o�^����
	void integrate(ID3D11DeviceContext* dc, SetParticleData data, int count);


	//�Q�b�g�A�Z�b�g
	int GetNowCount() { return static_cast<int>(now_count); }
	int GetUseCount() { return static_cast<int>(use_count); }


	//�f�o�b�N�pGUI�`��
	void DrawDebugGUI();

private:
	//�`��o�b�t�@�֓o�^
	void Register(ID3D11DeviceContext* dc);

private:

	particle_constants particle_data;
	size_t max_particle_count;

	//���g�p�̐�
	UINT now_count = 0;

	//���ݎg���Ă���J�E���g��
	UINT use_count = 0;

	//�o�^��
	UINT regist_count = 0;


	//�`��A�X�V�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_append_buffer_uav;

	//�o�^�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> register_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> register_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> register_particle_append_buffer_uav;

	//�J�E���g�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> now_count_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> now_count_particle_counter_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> now_count_particle_append_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_count_buffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_buffer_srv;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> init_compute_shader;		//�`��p�o�b�t�@�̏�����
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> set_compute_shader;			//�o�^�p�o�b�t�@�ɒl������
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> register_compute_shader;	//�`��p�o�b�t�@�ɓo�^
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_compute_shader;		//�`��p�o�b�t�@�̍X�V
	
	//�T�u�G�~�b�^�[�p�R���s���[�g�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> set_one_compute_shader;			//�o�^�p�o�b�t�@�ɒl������
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> register_one_compute_shader;	//�`��p�o�b�t�@�ɓo�^


	//�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_constant_buffer;


	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;

	//�e�N�X�`��
	std::unique_ptr<Texture> texture[3];
};


spread_particle::ParticleData SetDefaultParticleData();