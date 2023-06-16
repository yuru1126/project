#include "Graphics/ProjectionTextureManager.h"

//更新処理
void ProjectionTextureManager::Update(float elapsedTime)
{
    //破棄処理
    for (ProjectionTexture* projection_texture : removes)
    {
        // std::vectから要素を削除する場合はイテレーターで削除しなければならない
        std::vector<ProjectionTexture*>::iterator it = std::find(projection_textures.begin(), projection_textures.end(), projection_texture);

        if (it != projection_textures.end())
        {
            projection_textures.erase(it);
        }
        // テクスチャの破棄処理
        delete projection_texture;
    }
    //破棄リストをクリア
    removes.clear();
}

//テクスチャ登録
void ProjectionTextureManager::Register(ProjectionTexture* projection_texture)
{
    projection_textures.emplace_back(projection_texture);
}

//テクスチャ削除
void ProjectionTextureManager::Remove(ProjectionTexture* projection_texture)
{
    //破棄リストに追加
    removes.emplace_back(projection_texture);
}

//テクスチャ全消去
void ProjectionTextureManager::Clear()
{
    for (ProjectionTexture* projection_texture : projection_textures)
    {
        if (projection_texture != nullptr) { delete projection_texture; }
    }

    //破棄リストをクリア
    removes.clear();
}

void ProjectionTextureManager::DrawDebugGUI()
{
    for (ProjectionTexture* projection_texture : projection_textures)
    {
        projection_texture->DrawDebugGUI();
    }
}