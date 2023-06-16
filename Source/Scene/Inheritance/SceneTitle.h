#pragma once

#include "Scene/Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Shader/Shader2D.h"
#include "Graphics/Texture.h"

//�^�C�g���V�[��
class SceneTitle :public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle() override {}

    //������
    void Initialize() override;

    //�I����
    void Finalize()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //�`�揈��
    void Render()override;

private:

    std::unique_ptr<Shader2D> shader2D;
    std::unique_ptr<Sprite> sprite;
    std::unique_ptr<Texture> texture;
};