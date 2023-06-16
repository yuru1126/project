#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Scene/SceneManager.h"
#include "Scene/Inheritance/SceneLoading.h"
#include <thread>
#include "Misc.h"

//初期化
void SceneLoading::Initialize()
{
    //スプライト初期化
    sprite = std::make_unique<beforeSprite>("Data/Sprite/LoadingIcon.png");

    //スレッド開始
    std::thread thread(LoadingThread, this);
    //スレッドの管理を破棄
    thread.detach();
}

//終了化
void SceneLoading::Finalize()
{
}

//更新処理
void SceneLoading::Update(float elapsedTime)
{
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    GamePad& gamePad = Input::Instance().GetGamePad();
}

//ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    //COM関連の初期化でスレッド毎に呼ぶ必要かある
    HRESULT hr = CoInitialize(nullptr);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //次のシーンの初期化を行う
    scene->nextScene->Initialize();

    //スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    //次のシーンの準備完了設定
    scene->nextScene->SetRendeay();
}

//描画処理
void SceneLoading::Render()
{

    //次のシーンの準備が完了したらシーンを切り替える
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

    //画面クリア & レンダーターゲット設定
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f }; //RGBA (0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //2Dスプライト描画
    {
        //画面右下にローディングアイコンを描画
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