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

    //������
    void init(ID3D11DeviceContext* dc);

    //�X�V����
    void updete(ID3D11DeviceContext* dc, float elapsedTime);

    //�`�揈��
    void render(ID3D11DeviceContext* dc);


    //�p�[�e�B�N���Z�b�g
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
        float omega = 1.8f;     ///SOR�@�Ŏg�������W��
        float re = 500.0f;    //���C�m���Y��
        float dummy{};
    };
    CbFluid cb_data;

private:
    //X�����p�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> velocityX_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> velocityX_buffer_uav;

    //Y�����p�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> velocityY_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> velocityY_buffer_uav;

    //���U�p�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> divergence_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> divergence_buffer_uav;

    //���͗p�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> pressure_buffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressure_buffer_uav;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fluid_simulation_buffer_srv;


    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_init;        //������
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_adve;        //�ڗ��v�Z
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_copy;        //velocity��after����
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_viscosity;   //�S��
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_force;       //�O�͌v�Z
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_wall_zero;   //�Ǒ��x��0�ɌŒ�
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_div;         //�_�C�o�[�W�F���X�v�Z
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_poisson;     //�|�A�\���������̍�
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader_fix;         //�C��

    //�萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
};