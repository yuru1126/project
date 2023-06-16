#include "FluidSimulation/FluidSimulation.h"

#include <random>
#include "Misc.h"
#include <imgui.h>

#include "Input/Input.h"

FluidSimulation::FluidSimulation(ID3D11Device* device)
{
    HRESULT hr{ S_OK };
    {
        D3D11_BUFFER_DESC buffer_desc{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Velocity) * 13 * 12 * 1);
        buffer_desc.StructureByteStride = sizeof(Velocity);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        hr = device->CreateBuffer(&buffer_desc, NULL, velocityX_buffer.GetAddressOf());
        hr = device->CreateBuffer(&buffer_desc, NULL, velocityY_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(float) * 12 * 12 * 1);
        buffer_desc.StructureByteStride = sizeof(float);
        hr = device->CreateBuffer(&buffer_desc, NULL, pressure_buffer.GetAddressOf());
        hr = device->CreateBuffer(&buffer_desc, NULL, divergence_buffer.GetAddressOf());

        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    //シェーダ リソース ビュー作成
    {

    }

    // アンオーダード・アクセス・ビュー作成
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc{};
        unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
        unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        unordered_access_view_desc.Buffer.FirstElement = 0;
        unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(13 * 12 * 1);
        unordered_access_view_desc.Buffer.Flags = 0;
        hr = device->CreateUnorderedAccessView(velocityX_buffer.Get(), &unordered_access_view_desc, velocityX_buffer_uav.GetAddressOf());
        hr = device->CreateUnorderedAccessView(velocityY_buffer.Get(), &unordered_access_view_desc, velocityY_buffer_uav.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(12 * 12 * 1);
        hr = device->CreateUnorderedAccessView(pressure_buffer.Get(), &unordered_access_view_desc, pressure_buffer_uav.GetAddressOf());
        hr = device->CreateUnorderedAccessView(divergence_buffer.Get(), &unordered_access_view_desc, divergence_buffer_uav.GetAddressOf());

    }

    // 定数バッファ生成
    {
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = sizeof(CbFluid);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;
        hr = device->CreateBuffer(&desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // コンピュートシェーダー作成
    {
        create_cs_from_cso(device, "Shader\\FluidSimulationCSInit.cso", compute_shader_init.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSAdve.cso", compute_shader_adve.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSCopy.cso", compute_shader_copy.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSViscosity.cso", compute_shader_viscosity.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSForce.cso", compute_shader_force.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSWallZero.cso", compute_shader_wall_zero.ReleaseAndGetAddressOf());

        create_cs_from_cso(device, "Shader\\FluidSimulationCSDiv.cso", compute_shader_div.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSPoisson.cso", compute_shader_poisson.ReleaseAndGetAddressOf());
        create_cs_from_cso(device, "Shader\\FluidSimulationCSFix.cso", compute_shader_fix.ReleaseAndGetAddressOf());
    }
}

//初期化
void FluidSimulation::init(ID3D11DeviceContext* dc)
{
    HRESULT hr{ S_OK };
    UINT velocity_counts = 13 * 12 * 1;
    UINT divergences_counts = 12 * 12 * 1;

    dc->CSSetUnorderedAccessViews(0, 1, velocityX_buffer_uav.GetAddressOf(), &velocity_counts);
    dc->CSSetUnorderedAccessViews(1, 1, velocityY_buffer_uav.GetAddressOf(), &velocity_counts);
    dc->CSSetUnorderedAccessViews(2, 1, divergence_buffer_uav.GetAddressOf(), &divergences_counts);
    dc->CSSetUnorderedAccessViews(3, 1, pressure_buffer_uav.GetAddressOf(), &divergences_counts);
    dc->CSSetShader(compute_shader_init.Get(), NULL, 0);
    dc->Dispatch(1, 1, 1);

    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(2, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(3, 1, &null_unordered_access_view, nullptr);
}

//更新処理
void FluidSimulation::updete(ID3D11DeviceContext* dc, float elapsedTime)
{
    cb_data.elapsed_time = elapsedTime;
    cb_data.elapsed_time = 0.2f;
    dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &cb_data, 0, 0);
    dc->CSSetConstantBuffers(8, 1, constant_buffer.GetAddressOf());

    UINT velocity_counts = 13 * 12 * 1;
    UINT divergences_counts = 12 * 12 * 1;
    dc->CSSetUnorderedAccessViews(0, 1, velocityX_buffer_uav.GetAddressOf(), &velocity_counts);
    dc->CSSetUnorderedAccessViews(1, 1, velocityY_buffer_uav.GetAddressOf(), &velocity_counts);
    dc->CSSetUnorderedAccessViews(2, 1, divergence_buffer_uav.GetAddressOf(), &divergences_counts);
    dc->CSSetUnorderedAccessViews(3, 1, pressure_buffer_uav.GetAddressOf(), &divergences_counts);

    //移流計算
    {
        dc->CSSetShader(compute_shader_adve.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
        dc->CSSetShader(compute_shader_copy.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
    }

    // 粘性
    {
        dc->CSSetShader(compute_shader_viscosity.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
        dc->CSSetShader(compute_shader_copy.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
    }

    //外力
    {
        GamePad& gamePad = Input::Instance().GetGamePad();
        if (gamePad.GetButton() & GamePad::BTN_R)
        {
            dc->CSSetShader(compute_shader_force.Get(), NULL, 0);
            dc->Dispatch(1, 1, 1);
        }
    }

    //壁速度を0に固定
    {
        dc->CSSetShader(compute_shader_wall_zero.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
    }

    // ダイバージェンス計算
    {
        dc->CSSetShader(compute_shader_div.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
    }

    // ポアソン方程式の項
    {
        dc->CSSetShader(compute_shader_poisson.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
    }

    // 修正
    {
        dc->CSSetShader(compute_shader_fix.Get(), NULL, 0);
        dc->Dispatch(1, 1, 1);
    }


    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(1, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(2, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(3, 1, &null_unordered_access_view, nullptr);
}

//描画処理
void FluidSimulation::render(ID3D11DeviceContext* dc)
{
    
}


void FluidSimulation::Begin(ID3D11DeviceContext* dc)
{
    UINT velocity_counts = 13 * 12 * 1;
    dc->CSSetUnorderedAccessViews(5, 1, velocityX_buffer_uav.GetAddressOf(), &velocity_counts);
    dc->CSSetUnorderedAccessViews(6, 1, velocityY_buffer_uav.GetAddressOf(), &velocity_counts);
}

void FluidSimulation::End(ID3D11DeviceContext* dc)
{
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(5, 1, &null_unordered_access_view, nullptr);
    dc->CSSetUnorderedAccessViews(6, 1, &null_unordered_access_view, nullptr);
}