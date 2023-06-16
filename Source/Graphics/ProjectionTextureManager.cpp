#include "Graphics/ProjectionTextureManager.h"

//�X�V����
void ProjectionTextureManager::Update(float elapsedTime)
{
    //�j������
    for (ProjectionTexture* projection_texture : removes)
    {
        // std::vect����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        std::vector<ProjectionTexture*>::iterator it = std::find(projection_textures.begin(), projection_textures.end(), projection_texture);

        if (it != projection_textures.end())
        {
            projection_textures.erase(it);
        }
        // �e�N�X�`���̔j������
        delete projection_texture;
    }
    //�j�����X�g���N���A
    removes.clear();
}

//�e�N�X�`���o�^
void ProjectionTextureManager::Register(ProjectionTexture* projection_texture)
{
    projection_textures.emplace_back(projection_texture);
}

//�e�N�X�`���폜
void ProjectionTextureManager::Remove(ProjectionTexture* projection_texture)
{
    //�j�����X�g�ɒǉ�
    removes.emplace_back(projection_texture);
}

//�e�N�X�`���S����
void ProjectionTextureManager::Clear()
{
    for (ProjectionTexture* projection_texture : projection_textures)
    {
        if (projection_texture != nullptr) { delete projection_texture; }
    }

    //�j�����X�g���N���A
    removes.clear();
}

void ProjectionTextureManager::DrawDebugGUI()
{
    for (ProjectionTexture* projection_texture : projection_textures)
    {
        projection_texture->DrawDebugGUI();
    }
}