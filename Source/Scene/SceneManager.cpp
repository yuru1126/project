#include "Scene/SceneManager.h"

//�X�V����
void SceneManager::Update(float elapsedTime)
{
    if (currentScene != nullptr)
    {
        currentScene->Update(elapsedTime);
    }
}

//�`�揈��
void SceneManager::Render()
{
    if (currentScene != nullptr)
    {
        currentScene->Render();
    }
}

//�V�[���N���A
void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

//�V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    //�Â��V�[���̏I������
    Clear();

    //�V�����V�[����ݒ�
    currentScene = scene;

    //�V�[������������
    //���������̏ꍇ�͏���������
    if (!scene->Istrady())
    {
        currentScene->Initialize();
    }
}