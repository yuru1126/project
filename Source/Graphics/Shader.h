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

	// 描画開始
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// 描画
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	// 定数バッファ更新
	virtual void SetConstants(ID3D11DeviceContext* dc, const RenderContext& rc) {}

	// 描画終了
	virtual void End(ID3D11DeviceContext* context) = 0;
};

//ModelShader
class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	// 描画開始
	virtual void Begin(const RenderContext& rc) = 0;

	// 必要なバッファの設定
	virtual void SetBuffers(const RenderContext& rc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh) = 0;

	// サブセット単位で描画
	virtual void DrawSubset(const RenderContext& rc, const ModelResource::Subset& subset) = 0;

	// 描画終了
	virtual void End(const RenderContext& rc) = 0;
};

class SpriteShader
{
public:
	SpriteShader() {}
	virtual ~SpriteShader() {}

	// 描画開始
	virtual void Begin(const RenderContext& rc) = 0;

	// 描画
	virtual void Draw(const RenderContext& rc, const Sprite* sprite) = 0;

	// 描画終了
	virtual void End(const RenderContext& rc) = 0;
};


// 頂点シェーダー作成
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

// ピクセルシェーダー作成
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

// ジオメトリシェーダー作成
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);

// コンピュートシェーダー作成
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

// ハルシェーダー作成
HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);

// ドメインシェーダー作成
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);


HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);