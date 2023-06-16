#pragma once

#include "Scene/Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Shader/Shader2D.h"
#include "Graphics/Texture.h"

//タイトルシーン
class SceneTitle :public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle() override {}

    //初期化
    void Initialize() override;

    //終了化
    void Finalize()override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画処理
    void Render()override;

private:

    std::unique_ptr<Shader2D> shader2D;
    std::unique_ptr<Sprite> sprite;
    std::unique_ptr<Texture> texture;
};