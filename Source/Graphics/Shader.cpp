#include "Graphics/Shader.h"
#include "Misc.h"

#include <memory>
using namespace std;

// ���_�V�F�[�_�[�쐬
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
	// �t�@�C�����J��
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	// ���_�V�F�[�_�[����
	HRESULT hr{ S_OK };
	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	if (input_layout)
	{
		hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	return hr;
}

// �s�N�Z���V�F�[�_�[�쐬
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	// �t�@�C�����J��
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	// �s�N�Z���V�F�[�_�[����
	HRESULT hr{ S_OK };
	hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �W�I���g���V�F�[�_�[�쐬
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	// �W�I���g���V�F�[�_�[����
	HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �R���s���[�g�V�F�[�_�[�쐬
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	// �R���s���[�g�V�F�[�_�[����
	HRESULT hr = device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, compute_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �n���V�F�[�_�[�쐬
HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	// �n���V�F�[�_�[����
	HRESULT hr = device->CreateHullShader(cso_data.get(), cso_sz, nullptr, hull_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �h���C���V�F�[�_�[�쐬
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	// �h���C���V�F�[�_�[����
	HRESULT hr = device->CreateDomainShader(cso_data.get(), cso_sz, nullptr, domain_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

#include <WICTextureLoader.h>
using namespace DirectX;
using namespace Microsoft::WRL;
#include <map>

static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;
HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else
	{
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		resources.insert(make_pair(filename, *shader_resource_view));
	}

	ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}