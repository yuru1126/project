#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>
#include<EffekseerRendererDX11.h>

//�G�t�F�N�g�}�l�[�W���[
class EffectManager
{
private:
    EffectManager() {}
    ~EffectManager() {}
public:
    //�B��̃C���^�[�t�F�[�X�擾
    static EffectManager& Instance()
    {
        static EffectManager instance;
        return instance;
    }
    //������
    void Initalize();

    //�I����
    void Finalize();

    //�X�V����
    void Update(float elapsedTime);
   
    //�`�揈��
    void Render(const DirectX::XMFLOAT4X4& view,const DirectX::XMFLOAT4X4& projection);

    //Effeckseer�}�l�[�W���[�̎擾
    Effekseer::Manager* GetEffseerManager() { return effekseerManager; }

private:
    Effekseer::Manager*             effekseerManager = nullptr;
    EffekseerRenderer::Renderer*    effekseerRenderer = nullptr;
};