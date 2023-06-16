#pragma once
#include <d3d11.h>
#include "Graphics/Shader/Shader2D.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/Shader/ProjectionTextureMapShader.h"
#include "Graphics/Texture.h"

class Gbuffer
{
public:
	Gbuffer();
	~Gbuffer();
	void Initialize();
	void Activate();
	void DepthActivate();
	void Deactivate();
	void Render();

	void DrawDebugGUI();

protected:
	std::unique_ptr<Shader2D> PostEffectShader;
	std::unique_ptr<Sprite> ScreenSprite;

	std::unique_ptr<DepthStencil> depthStencil;	// �[�x�o�b�t�@
	//���e�e�N�X�`���p
	std::unique_ptr<DepthStencil> texture_map_depthStencil;

	// �J���[�o�b�t�@
	std::unique_ptr<RenderTarget> Color;
	// �[�x�o�b�t�@(�J��������̋���)
	std::unique_ptr<RenderTarget> Depth;
	// �@��
	std::unique_ptr<RenderTarget> Normal;
	// �R�c���W
	std::unique_ptr<RenderTarget> Position;
	//Roughness + Metallic
	std::unique_ptr<RenderTarget> RM;

	//���e�e�N�X�`���p �[�x�o�b�t�@
	std::unique_ptr<RenderTarget> ProjectionDepth;

protected:
	// ���C�g���ʗp�o�b�t�@
	std::unique_ptr<RenderTarget> LightBuffer;
	std::unique_ptr<RenderTarget> Composite;


	void Lighting();
	std::unique_ptr<Shader2D> LightShadowShader;
	std::unique_ptr<Shader2D> LightShader;
	std::unique_ptr<Shader2D> EnvLightShader;
	std::unique_ptr<Shader2D> CompositeShader;

	struct CBLightParam 
	{
		DirectX::XMFLOAT4 Direction;	//����
		DirectX::XMFLOAT4 Color;		//�F
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> CBLight;

protected:
	struct CbScene 
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	viewPosition;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;

private:

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[2];
	// Bloom
	std::unique_ptr<Shader2D> bloom_shader;

	std::unique_ptr<RenderTarget> bloom_texture[4];
	DirectX::XMFLOAT2 bloom_texture_size[5]{};
	struct CbBloom
	{
		DirectX::XMFLOAT3	set_color = { 0.299f,0.587f,0.114f };
		float min = 0.2f;
		float max = 0.8f;
		DirectX::XMFLOAT3  dummy{};
	};

	CbBloom cb_bioom;
	Microsoft::WRL::ComPtr<ID3D11Buffer> bloom_constant_buffer;


	// DOF
	std::unique_ptr<RenderTarget> DOFbloom_texture;
	std::unique_ptr<Shader2D> DOFbloom_shader;

	//���e�e�N�X�`��
	std::unique_ptr<ProjectionTextureMapShader> texture_map_shader;

	std::unique_ptr<Texture> texture;
};

