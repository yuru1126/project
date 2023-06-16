#include "Graphics/Graphics.h"
#include "Scene/Inheritance/SceneTitle.h"
#include "Scene/Inheritance/SceneLoading.h"
#include "Scene/Inheritance/SceneExhibition.h"
#include "Scene/SceneManager.h"
#include "Input/Input.h"

//������
void SceneTitle::Initialize()
{
    Graphics& graphics = Graphics::Instance();
    shader2D = std::make_unique<Shader2D>(graphics.GetDevice());
    shader2D->Load("SpritePS");

    texture = std::make_unique<Texture>("Data/Sprite/Title.png");
    //�X�v���C�g������
    sprite = std::make_unique<Sprite>();
    sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());
    float screenWidth = static_cast<float>(graphics.GetScreenWidth());
    float screenHeight = static_cast<float>(graphics.GetScreenHeight());
    float textureWidth = static_cast<float>(texture->GetWidth());
    float textureHeigt = static_cast<float>(texture->GetHeight());
    sprite->Update(
        0, 0, screenWidth, screenHeight,
        0, 0, textureWidth, textureHeigt,
        0,
        1, 1, 1, 1);
}

//�I����
void SceneTitle::Finalize()
{
}

//�X�V����
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    //�����{�^������������Q�[���V�[���֐؂�ւ�
    const GamePadButton anyButton =
        GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_X
        | GamePad::BTN_Y;

    if (gamePad.GetButtonDown() & anyButton)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneExhibition));
    }

}

//�`�揈��
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    //��ʃN���A & �����_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f }; //RGBA (0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);
    
    //2D�`��
    {
        RenderContext rc;
        rc.deviceContext = dc;
        shader2D->Begin(rc);
        shader2D->Draw(rc, sprite.get());
        shader2D->End(rc);
    }
}