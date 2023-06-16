#include "Graphics/Graphics.h"
#include "Scene/Inheritance/SceneTitle.h"
#include "Scene/Inheritance/SceneLoading.h"
#include "Scene/Inheritance/SceneExhibition.h"
#include "Scene/SceneManager.h"
#include "Input/Input.h"

//初期化
void SceneTitle::Initialize()
{
    Graphics& graphics = Graphics::Instance();
    shader2D = std::make_unique<Shader2D>(graphics.GetDevice());
    shader2D->Load("SpritePS");

    texture = std::make_unique<Texture>("Data/Sprite/Title.png");
    //スプライト初期化
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

//終了化
void SceneTitle::Finalize()
{
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    //何かボタンを押したらゲームシーンへ切り替え
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

//描画処理
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    //画面クリア & レンダーターゲット設定
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f }; //RGBA (0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);
    
    //2D描画
    {
        RenderContext rc;
        rc.deviceContext = dc;
        shader2D->Begin(rc);
        shader2D->Draw(rc, sprite.get());
        shader2D->End(rc);
    }
}