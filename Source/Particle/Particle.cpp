#include "Particle/Particle.h"

#include <random>
#include "Misc.h"
#include <imgui.h>

spread_particle::spread_particle(ID3D11Device* device, size_t max_particle_count) : max_particle_count(max_particle_count)
{
	HRESULT hr{ S_OK };

	{
		D3D11_BUFFER_DESC buffer_desc{};

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(particle) * max_particle_count);
		buffer_desc.StructureByteStride = sizeof(particle);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		hr = device->CreateBuffer(&buffer_desc, NULL, particle_buffer.GetAddressOf());
		hr = device->CreateBuffer(&buffer_desc, NULL, register_particle_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//カウント用
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * max_particle_count);
		buffer_desc.StructureByteStride = sizeof(uint32_t);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		hr = device->CreateBuffer(&buffer_desc, NULL, now_count_particle_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t));
		buffer_desc.StructureByteStride = sizeof(uint32_t);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		buffer_desc.MiscFlags = 0;
		hr = device->CreateBuffer(&buffer_desc, NULL, particle_count_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//シェーダ リソース ビュー作成
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		shader_resource_view_desc.Buffer.ElementOffset = 0;
		shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		hr = device->CreateShaderResourceView(particle_buffer.Get(),
			&shader_resource_view_desc, particle_buffer_srv.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// アンオーダード・アクセス・ビュー作成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc{};
		unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		unordered_access_view_desc.Buffer.FirstElement = 0;
		unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		unordered_access_view_desc.Buffer.Flags = 0;
		hr = device->CreateUnorderedAccessView(particle_buffer.Get(), &unordered_access_view_desc, particle_buffer_uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(register_particle_buffer.Get(), &unordered_access_view_desc, register_particle_buffer_uav.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		unordered_access_view_desc.Buffer.FirstElement = 0;
		unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(particle_buffer.Get(), &unordered_access_view_desc, particle_append_buffer_uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(register_particle_buffer.Get(), &unordered_access_view_desc, register_particle_append_buffer_uav.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//カウント用
		unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		unordered_access_view_desc.Buffer.FirstElement = 0;
		unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(now_count_particle_buffer.Get(), &unordered_access_view_desc, now_count_particle_counter_buffer_uav.GetAddressOf());

		unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(now_count_particle_buffer.Get(), &unordered_access_view_desc, now_count_particle_append_buffer_uav.GetAddressOf());
	}

	// 定数バッファ生成
	{
		//パーティクルバッファ
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(particle_constants);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		
		//シーンバッファ
		desc.ByteWidth = sizeof(CbScene);
		hr = device->CreateBuffer(&desc, nullptr, scene_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//パーティクルデータバッファ
		desc.ByteWidth = sizeof(SetParticleData);
		hr = device->CreateBuffer(&desc, nullptr, particle_constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}


	// 頂点シェーダー作成
	{
		create_vs_from_cso(device, "Shader\\ParticleVS.cso",vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	}

	// ピクセルシェーダー作成
	{
		create_ps_from_cso(device, "Shader\\ParticlePS.cso",pixel_shader.ReleaseAndGetAddressOf());
	}

	// ジオメトリシェーダー作成
	{
		create_gs_from_cso(device, "Shader\\ParticleGS.cso", geometry_shader.ReleaseAndGetAddressOf());
	}

	// コンピュートシェーダー作成
	{
		create_cs_from_cso(device, "Shader\\ParticleCSSet.cso", set_compute_shader.ReleaseAndGetAddressOf());

		create_cs_from_cso(device, "Shader\\ParticleCSUpdate.cso",update_compute_shader.ReleaseAndGetAddressOf());

		create_cs_from_cso(device, "Shader\\ParticleCSRegister.cso",register_compute_shader.ReleaseAndGetAddressOf());

		create_cs_from_cso(device, "Shader\\ParticleCSInit.cso",init_compute_shader.ReleaseAndGetAddressOf());

		//サブエミッター用
		create_cs_from_cso(device, "Shader\\ParticleCSSetOne.cso", set_one_compute_shader.ReleaseAndGetAddressOf());
		create_cs_from_cso(device, "Shader\\ParticleCSRegisterOne.cso", register_one_compute_shader.ReleaseAndGetAddressOf());
	}

	//テクスチャ生成
	{
		texture[0] = std::make_unique<Texture>("Data/Sprite/particle_circle2.png");
		texture[1] = std::make_unique<Texture>("Data/Sprite/pa.png");
		texture[2] = std::make_unique<Texture>("Data/Sprite/particle.png");
	}

	//ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = true;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

spread_particle::spread_particle(ID3D11Device* device, size_t max_particle_count, const char* CSUpdete) : max_particle_count(max_particle_count)
{
	HRESULT hr{ S_OK };

	{
		D3D11_BUFFER_DESC buffer_desc{};

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(particle) * max_particle_count);
		buffer_desc.StructureByteStride = sizeof(particle);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		hr = device->CreateBuffer(&buffer_desc, NULL, particle_buffer.GetAddressOf());
		hr = device->CreateBuffer(&buffer_desc, NULL, register_particle_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//カウント用
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * max_particle_count);
		buffer_desc.StructureByteStride = sizeof(uint32_t);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		hr = device->CreateBuffer(&buffer_desc, NULL, now_count_particle_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t));
		buffer_desc.StructureByteStride = sizeof(uint32_t);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		buffer_desc.MiscFlags = 0;
		hr = device->CreateBuffer(&buffer_desc, NULL, particle_count_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//シェーダ リソース ビュー作成
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		shader_resource_view_desc.Buffer.ElementOffset = 0;
		shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		hr = device->CreateShaderResourceView(particle_buffer.Get(),
			&shader_resource_view_desc, particle_buffer_srv.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// アンオーダード・アクセス・ビュー作成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc{};
		unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		unordered_access_view_desc.Buffer.FirstElement = 0;
		unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		unordered_access_view_desc.Buffer.Flags = 0;
		hr = device->CreateUnorderedAccessView(particle_buffer.Get(), &unordered_access_view_desc, particle_buffer_uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(register_particle_buffer.Get(), &unordered_access_view_desc, register_particle_buffer_uav.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		unordered_access_view_desc.Buffer.FirstElement = 0;
		unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(particle_buffer.Get(), &unordered_access_view_desc, particle_append_buffer_uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(register_particle_buffer.Get(), &unordered_access_view_desc, register_particle_append_buffer_uav.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//カウント用
		unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
		unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		unordered_access_view_desc.Buffer.FirstElement = 0;
		unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(max_particle_count);
		unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(now_count_particle_buffer.Get(), &unordered_access_view_desc, now_count_particle_counter_buffer_uav.GetAddressOf());

		unordered_access_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(now_count_particle_buffer.Get(), &unordered_access_view_desc, now_count_particle_append_buffer_uav.GetAddressOf());
	}

	// 定数バッファ生成
	{
		//パーティクルバッファ
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(particle_constants);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//シーンバッファ
		desc.ByteWidth = sizeof(CbScene);
		hr = device->CreateBuffer(&desc, nullptr, scene_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//パーティクルデータバッファ
		desc.ByteWidth = sizeof(SetParticleData);
		hr = device->CreateBuffer(&desc, nullptr, particle_constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}


	// 頂点シェーダー作成
	{
		create_vs_from_cso(device, "Shader\\ParticleVS.cso", vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	}

	// ピクセルシェーダー作成
	{
		create_ps_from_cso(device, "Shader\\ParticlePS.cso", pixel_shader.ReleaseAndGetAddressOf());
	}

	// ジオメトリシェーダー作成
	{
		create_gs_from_cso(device, "Shader\\ParticleGS.cso", geometry_shader.ReleaseAndGetAddressOf());
	}

	// コンピュートシェーダー作成
	{
		create_cs_from_cso(device, "Shader\\ParticleCSSet.cso", set_compute_shader.ReleaseAndGetAddressOf());

		create_cs_from_cso(device, "Shader\\ParticleCSRegister.cso", register_compute_shader.ReleaseAndGetAddressOf());

		create_cs_from_cso(device, "Shader\\ParticleCSInit.cso", init_compute_shader.ReleaseAndGetAddressOf());

		//サブエミッター用
		create_cs_from_cso(device, "Shader\\ParticleCSSetOne.cso", set_one_compute_shader.ReleaseAndGetAddressOf());
		create_cs_from_cso(device, "Shader\\ParticleCSRegisterOne.cso", register_one_compute_shader.ReleaseAndGetAddressOf());


		//更新用コンピュートシェーダーを変更
		{
			FILE* fp = nullptr;
			char filename[256];
			sprintf_s(filename, 256, "Shader/%s.cso", CSUpdete);
		
			create_cs_from_cso(device, filename, update_compute_shader.ReleaseAndGetAddressOf());
		}

	}

	//テクスチャ生成
	{
		texture[0] = std::make_unique<Texture>("Data/Sprite/particle_circle2.png");
		texture[1] = std::make_unique<Texture>("Data/Sprite/pa.png");
		texture[2] = std::make_unique<Texture>("Data/Sprite/particle.png");
	}

	//ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = true;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}


UINT align(UINT num, UINT alignment)
{
	return (num + (alignment - 1)) & ~(alignment - 1);
}

void spread_particle::input(ID3D11DeviceContext* dc, const RenderContext& rc)
{

	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
	cbScene.viewPosition = rc.viewPosition;

	rc.deviceContext->UpdateSubresource(scene_buffer.Get(), 0, 0, &cbScene, 0, 0);
}

//初期化
void spread_particle::init(ID3D11DeviceContext* dc)
{

	HRESULT hr{ S_OK };
	UINT counts = 0;
	dc->CSSetUnorderedAccessViews(1, 1, particle_append_buffer_uav.GetAddressOf(), &counts);
	dc->CSSetUnorderedAccessViews(4, 1, now_count_particle_append_buffer_uav.GetAddressOf(), &counts);

	dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	dc->CSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	dc->CSSetConstantBuffers(1, 1, scene_buffer.GetAddressOf());
	dc->CSSetShader(init_compute_shader.Get(), NULL, 0);
	//パーティクルを256の倍数になるように生成
	UINT num_threads = align(static_cast<UINT>(max_particle_count - 256), 256);
	dc->Dispatch(num_threads / 256, 1, 1);

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
	dc->CSSetUnorderedAccessViews(4, 1, &null_unordered_access_view, nullptr);

	//現在の登録数を取得
	dc->CopyStructureCount(particle_count_buffer.Get(), 0, particle_append_buffer_uav.Get());
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	UINT count{ *reinterpret_cast<UINT*>(mapped_subresource.pData) };
	dc->Unmap(particle_count_buffer.Get(), 0);

	particle_data.particle_count = count;

	//最大数から未使用数を引き現在使用数を算出
	dc->CopyStructureCount(particle_count_buffer.Get(), 0, now_count_particle_append_buffer_uav.Get());
	hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	UINT data_count{ *reinterpret_cast<UINT*>(mapped_subresource.pData) };
	dc->Unmap(particle_count_buffer.Get(), 0);
	this->now_count = data_count;
	use_count = particle_data.particle_count - now_count;
}

//登録処理
void spread_particle::integrate(ID3D11DeviceContext* dc, SetParticleData data, int set_count)
{
	UINT set_counts = static_cast<UINT>(set_count);

	//登録数が未使用の数を上回る可能性があるなら return
	if (align(regist_count + set_counts, 128) > now_count) return;

	particle_data.progress_time += particle_data.elapsed_time * 10;

	HRESULT hr{ S_OK };
	//登録数より後に登録
	dc->CSSetUnorderedAccessViews(3, 1, register_particle_append_buffer_uav.GetAddressOf(), &regist_count);

	dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	dc->UpdateSubresource(particle_constant_buffer.Get(), 0, 0, &data, 0, 0);

	dc->CSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	dc->CSSetConstantBuffers(1, 1, scene_buffer.GetAddressOf());
	dc->CSSetConstantBuffers(2, 1, particle_constant_buffer.GetAddressOf());


	UINT num_threads;

	//登録数が128より小さければ1つずつ設定
	if (set_counts < 128)
	{
		dc->CSSetShader(set_one_compute_shader.Get(), NULL, 0);
		dc->Dispatch(set_counts, 1, 1);
	}
	else
	{
		UINT one_count = set_counts % 128;
		if (one_count != 0)
		{
			dc->CSSetShader(set_one_compute_shader.Get(), NULL, 0);
			dc->Dispatch(one_count, 1, 1);

			particle_data.progress_time += particle_data.elapsed_time * 10.0f;
			dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
		}

		dc->CSSetShader(set_compute_shader.Get(), NULL, 0);
		num_threads = set_counts / 128;
		dc->Dispatch(num_threads, 1, 1);
	}
	

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(3, 1, &null_unordered_access_view, nullptr);

	//登録数を更新
	dc->CopyStructureCount(particle_count_buffer.Get(), 0, register_particle_append_buffer_uav.Get());
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	UINT count{ *reinterpret_cast<UINT*>(mapped_subresource.pData) };
	dc->Unmap(particle_count_buffer.Get(), 0);

	regist_count = count;
	return;
}

//描画、更新用バッファへ登録
void spread_particle::Register(ID3D11DeviceContext* dc)
{
	//登録がなければ何もしない
	if (regist_count > 0)
	{
		HRESULT hr{ S_OK };
		D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
		ID3D11UnorderedAccessView* null_unordered_access_view{};
		UINT counts_zero = 0;

		dc->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), nullptr);
		dc->CSSetUnorderedAccessViews(2, 1, register_particle_buffer_uav.GetAddressOf(), &counts_zero);
		dc->CSSetUnorderedAccessViews(5, 1, now_count_particle_counter_buffer_uav.GetAddressOf(), &now_count);
		dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
		
		//登録数が128より小さければ1つずつ設定
		if (regist_count < 128)
		{
			dc->CSSetShader(register_one_compute_shader.Get(), NULL, 0);
			dc->Dispatch(regist_count, 1, 1);
		}
		else
		{
			UINT one_count = regist_count % 128;
			if (one_count != 0)
			{
				dc->CSSetShader(register_one_compute_shader.Get(), NULL, 0);
				dc->Dispatch(one_count, 1, 1);

				particle_data.count = one_count;
				dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
			}
			dc->CSSetShader(register_compute_shader.Get(), NULL, 0);
			UINT num_threads = regist_count / 128;
			dc->Dispatch(num_threads, 1, 1);

			particle_data.count = 0;
		}


		dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
		dc->CSSetUnorderedAccessViews(2, 1, &null_unordered_access_view, nullptr);
		dc->CSSetUnorderedAccessViews(5, 1, &null_unordered_access_view, nullptr);

		dc->CopyStructureCount(particle_count_buffer.Get(), 0, now_count_particle_counter_buffer_uav.Get());
		hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		UINT data_count{ *reinterpret_cast<UINT*>(mapped_subresource.pData) };
		dc->Unmap(particle_count_buffer.Get(), 0);
		this->now_count = data_count;
	}
	regist_count = 0;
	return;
}

//更新処理
void spread_particle::updete(ID3D11DeviceContext* dc, float elapsedTime)
{
	//描画、更新用バッファへ登録
	Register(dc);


	UINT counts = particle_data.particle_count;
	_ASSERT_EXPR(particle_data.particle_count <= max_particle_count, L"");
	if (particle_data.particle_count == 0)
	{
		return;
	}

	UINT counts_zero = 0;
	dc->CSSetUnorderedAccessViews(3, 1, register_particle_append_buffer_uav.GetAddressOf(), &counts_zero);

	dc->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), nullptr);
	dc->CSSetUnorderedAccessViews(4, 1, now_count_particle_append_buffer_uav.GetAddressOf(), &now_count);

	particle_data.elapsed_time = elapsedTime;
	particle_data.progress_time += elapsedTime;

	dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	dc->CSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	dc->CSSetConstantBuffers(1, 1, scene_buffer.GetAddressOf());
	dc->CSSetShader(update_compute_shader.Get(), NULL, 0);

	UINT num_threads = align(particle_data.particle_count, 256);
	//更新処理
	dc->Dispatch(num_threads / 256, 1, 1);

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
	dc->CSSetUnorderedAccessViews(4, 1, &null_unordered_access_view, nullptr);

	//現在の数を数える
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	dc->CopyStructureCount(particle_count_buffer.Get(), 0, now_count_particle_append_buffer_uav.Get());
	hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	UINT data_count{ *reinterpret_cast<UINT*>(mapped_subresource.pData) };
	dc->Unmap(particle_count_buffer.Get(), 0);
	this->now_count = data_count;
	use_count = particle_data.particle_count - now_count;

	//サブエミッターでの追加
	dc->CSSetUnorderedAccessViews(3, 1, &null_unordered_access_view, nullptr);
	dc->CopyStructureCount(particle_count_buffer.Get(), 0, register_particle_append_buffer_uav.Get());
	hr = dc->Map(particle_count_buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	UINT sub_count{ *reinterpret_cast<UINT*>(mapped_subresource.pData) };
	dc->Unmap(particle_count_buffer.Get(), 0);

	if (now_count >= sub_count && sub_count > 0)
	{
		regist_count = sub_count;
	}
}

//描画処理
void spread_particle::render(ID3D11DeviceContext* dc)
{
	_ASSERT_EXPR(particle_data.particle_count <= max_particle_count, L"");
	if (particle_data.particle_count == 0)
	{
		return;
	}

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);

	dc->VSSetShader(vertex_shader.Get(), NULL, 0);
	dc->PSSetShader(pixel_shader.Get(), NULL, 0);
	dc->GSSetShader(geometry_shader.Get(), NULL, 0);
	dc->GSSetShaderResources(9, 1, particle_buffer_srv.GetAddressOf());

	ID3D11ShaderResourceView* srvs[] =
	{
		 texture[0]->GetShaderResourceView().Get(),
		 texture[1]->GetShaderResourceView().Get(),
		 texture[2]->GetShaderResourceView().Get()
	};

	dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	dc->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	dc->PSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	dc->GSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	dc->GSSetConstantBuffers(1, 1, scene_buffer.GetAddressOf());

	dc->IASetInputLayout(NULL);
	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	dc->Draw(static_cast<UINT>(particle_data.particle_count), 0);

	ID3D11ShaderResourceView* null_shader_resource_view{};
	dc->GSSetShaderResources(9, 1, &null_shader_resource_view);
	dc->VSSetShader(NULL, NULL, 0);
	dc->PSSetShader(NULL, NULL, 0);
	dc->GSSetShader(NULL, NULL, 0);
}

//デバック用GUI描画
void spread_particle::DrawDebugGUI()
{
	ImGui::Text("now count %d", now_count);
	ImGui::Text("use count %d", use_count);
}


spread_particle::ParticleData SetDefaultParticleData()
{
	spread_particle::ParticleData data;
	data.position_max = { 0,0,0,0 };
	data.position_min = { 0,0,0,0 };
	data.velocity_max = { 1.0f,1.0f,1.0f,1.0f };
	data.velocity_min = { -1.0f,-1.0f,-1.0f,0.5f };
	
	data.color_max = { 0,0.75f,0.95f,1 };
	data.color_min = { 360.0f,0.75f,0.95f,1 };

	data.lifespan = { 1.5f,2.5f };
	data.set_divide_age = 1.0f;
	data.gravity = 0.05f;
	data.draw_size = 0.85f;
	data.animation_speed = 0.0f;
	data.length_shrink_flag = 1.0f;
	data.draw_type = 0;
	data.tiles = { 1,1,1 };
	data.set_tiles = { 0,0 };
	data.set_state = 0;
	
	data.hsv_data_flag = 1;
	
	data.velocity_normalize_flag = 1;

	return data;
}