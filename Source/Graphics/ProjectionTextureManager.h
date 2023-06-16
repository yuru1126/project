#pragma once
#include <vector>
#include "Graphics/ProjectionTexture.h"

//���e�e�N�X�`���}�l�[�W���[
class ProjectionTextureManager
{
private:
    ProjectionTextureManager() {}
    ~ProjectionTextureManager() {}

public:
    //�B��̃C���X�^���X�擾
    static ProjectionTextureManager& Instance()
    {
        static ProjectionTextureManager instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime);

    //�e�N�X�`���o�^
    void Register(ProjectionTexture* projection_texture);

    //�e�N�X�`���폜
    void Remove(ProjectionTexture* projection_texture);

    //�e�N�X�`���S����
    void Clear();

    //�e�N�X�`���擾
    int GetProjectionTextureCount() const { return static_cast<int>(projection_textures.size()); }

    //�e�N�X�`���擾
    ProjectionTexture* GetProjectionTexture(int index) { return projection_textures.at(index); }

    void DrawDebugGUI();

private:
    std::vector<ProjectionTexture*>     projection_textures;
    std::vector<ProjectionTexture*>     removes;
};