#pragma once

#include "Graphics/Sprite.h"
#include "Scene/Scene.h"

//���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
    SceneLoading() {}
    SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
    ~SceneLoading() override {}

    //������
    void Initialize() override;

    //�I����
    void Finalize() override;

    //�X�V����
    void Update(float elapsedTime) override;

    //�`�揈��
    void Render() override;

private:
    //���[�f�B���O�X���b�h
    static void LoadingThread(SceneLoading* scene);

private:
    std::unique_ptr<beforeSprite> sprite;
    float angle = 0.0f;
    Scene* nextScene = nullptr;
};