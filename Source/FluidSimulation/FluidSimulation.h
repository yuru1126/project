#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <functional>
#include "Graphics/Shader.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Texture.h"

class FluidSimulation
{
public:
    FluidSimulation(ID3D11Device* device);
    ~FluidSimulation() {}

    //初期化
    void init(ID3D11DeviceContext* dc);

    //更新処理
    void updete(ID3D11DeviceContext* dc, float elapsedTime);

    //描画処理
    void render(ID3D11DeviceContext* dc);


    //パーティクルセット
    void Begin(ID3D11DeviceContext* dc);
    void End(ID3D11DeviceContext* dc);

private:
    struct Velocity
    {
        float velocity;
        float after;
    };

private:
    struct CbFluid
    {
        float elapsed_time = 0.0f;
        float omega = 1.8f;     ///SOR法で使う加速係数
        float re = 500.0f;    //レイノルズ数
        float dummy{};
    };
    CbFluid cb_data;

private:
    //X方向用バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> velocityX_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> velocityX_buffer_uav;

    //Y方向用バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> velocityY_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> velocityY_buffer_uav;

    //発散用バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> divergence_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> divergence_buffer_uav;

    //圧力用バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> pressure_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressure_buffer_uav;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fluid_simulation_buffer_srv;


    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_init;        //初期化
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_adve;        //移流計算
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_copy;        //velocityにafterを代入
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_viscosity;   //粘性
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_force;       //外力計算
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_wall_zero;   //壁速度を0に固定
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_div;         //ダイバージェンス計算
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_poisson;     //ポアソン方程式の項
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_fix;         //修正

    //定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
};