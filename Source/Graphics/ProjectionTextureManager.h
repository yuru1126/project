#pragma once
#include <vector>
#include "Graphics/ProjectionTexture.h"

//投影テクスチャマネージャー
class ProjectionTextureManager
{
private:
    ProjectionTextureManager() {}
    ~ProjectionTextureManager() {}

public:
    //唯一のインスタンス取得
    static ProjectionTextureManager& Instance()
    {
        static ProjectionTextureManager instance;
        return instance;
    }

    //更新処理
    void Update(float elapsedTime);

    //テクスチャ登録
    void Register(ProjectionTexture* projection_texture);

    //テクスチャ削除
    void Remove(ProjectionTexture* projection_texture);

    //テクスチャ全消去
    void Clear();

    //テクスチャ取得
    int GetProjectionTextureCount() const { return static_cast<int>(projection_textures.size()); }

    //テクスチャ取得
    ProjectionTexture* GetProjectionTexture(int index) { return projection_textures.at(index); }

    void DrawDebugGUI();

private:
    std::vector<ProjectionTexture*>     projection_textures;
    std::vector<ProjectionTexture*>     removes;
};