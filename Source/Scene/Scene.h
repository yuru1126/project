#pragma once

//�V�[��
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //������
    virtual void Initialize() = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render() = 0;

    //�����������Ă��邩
    bool Istrady() const { return ready; }

    //���������ݒ�
    void SetRendeay() { ready = true; }

private:
    bool ready = false;

};