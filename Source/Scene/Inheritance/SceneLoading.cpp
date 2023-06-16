#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Scene/SceneManager.h"
#include "Scene/Inheritance/SceneLoading.h"
#include <thread>
#include "Misc.h"

//������
void SceneLoading::Initialize()
{
    //�X�v���C�g������
    sprite = std::make_unique<beforeSprite>("Data/Sprite/LoadingIcon.png");

    //�X���b�h�J�n
    std::thread thread(LoadingThread, this);
    //�X���b�h�̊Ǘ���j��
    thread.detach();
}

//�I����
void SceneLoading::Finalize()
{
}

//�X�V����
void SceneLoading::Update(float elapsedTime)
{
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    GamePad& gamePad = Input::Instance().GetGamePad();
}

//���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    //COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    HRESULT hr = CoInitialize(nullptr);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //���̃V�[���̏��������s��
    scene->nextScene->Initialize();

    //�X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    //���̃V�[���̏��������ݒ�
    scene->nextScene->SetRendeay();
}

//�`�揈��
void SceneLoading::Render()
{

    //���̃V�[���̏���������������V�[����؂�ւ���
    if (nextScene->Istrady())
    {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
        return;
    }

    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    //��ʃN���A & �����_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f }; //RGBA (0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //2D�X�v���C�g�`��
    {
        //��ʉE���Ƀ��[�f�B���O�A�C�R����`��
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        float textureWidth = static_cast<float>(sprite->GetTextureWidth());
        float textureHeigt = static_cast<float>(sprite->GetTextureHeight());
        float positionX = screenWidth - textureWidth;
        float positionY = screenHeight - textureHeigt;
        
        sprite->Render(dc,
            positionX, positionY, textureWidth, textureHeigt,
            0, 0, textureWidth, textureHeigt,
            angle,
            1, 1, 1, 1);
    }
}