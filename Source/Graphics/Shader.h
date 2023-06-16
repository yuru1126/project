#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/RenderContext.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Sprite.h"

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// �`��J�n
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// �`��
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	// �萔�o�b�t�@�X�V
	virtual void SetConstants(ID3D11DeviceContext* dc, const RenderContext& rc) {}

	// �`��I��
	virtual void End(ID3D11DeviceContext* context) = 0;
};

//ModelShader
class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	// �`��J�n
	virtual void Begin(const RenderContext& rc) = 0;

	// �K�v�ȃo�b�t�@�̐ݒ�
	virtual void SetBuffers(const RenderContext& rc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh) = 0;

	// �T�u�Z�b�g�P�ʂŕ`��
	virtual void DrawSubset(const RenderContext& rc, const ModelResource::Subset& subset) = 0;

	// �`��I��
	virtual void End(const RenderContext& rc) = 0;
};

class SpriteShader
{
public:
	SpriteShader() {}
	virtual ~SpriteShader() {}

	// �`��J�n
	virtual void Begin(const RenderContext& rc) = 0;

	// �`��
	virtual void Draw(const RenderContext& rc, const Sprite* sprite) = 0;

	// �`��I��
	virtual void End(const RenderContext& rc) = 0;
};


// ���_�V�F�[�_�[�쐬
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

// �s�N�Z���V�F�[�_�[�쐬
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

// �W�I���g���V�F�[�_�[�쐬
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);

// �R���s���[�g�V�F�[�_�[�쐬
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

// �n���V�F�[�_�[�쐬
HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);

// �h���C���V�F�[�_�[�쐬
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);


HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);