#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics/Model/ModelResource.h"
#include "Graphics/RenderContext.h"

//
#define MOVE_ANGLE FALS

// ���f��
class Model
{
public:
	Model(const char* filename);
	Model() {}
	virtual ~Model() {}

	enum BlendType
	{
		First,
		Second,
		Full
	};

	struct Node
	{
		const char* name;
		Node* parent;
		DirectX::XMFLOAT3	scale{};
		DirectX::XMFLOAT4	rotate{};
		DirectX::XMFLOAT3	translate{};
		DirectX::XMFLOAT4X4	localTransform{};
		DirectX::XMFLOAT4X4	worldTransform{};

		//�������̃u�����h�p
		//��(��{)�̈ʒu
		DirectX::XMFLOAT3	basic_scale{};
		DirectX::XMFLOAT4	basic_rotate{};
		DirectX::XMFLOAT3	basic_translate{};

		//�����p(��)Divide
		DirectX::XMFLOAT3	blend_scale{};
		DirectX::XMFLOAT4	blend_rotate{};
		DirectX::XMFLOAT3	blend_translate{};
		DirectX::XMFLOAT4X4	blend_localTransform{};
		DirectX::XMFLOAT4X4	blend_worldTransform{};


		DirectX::XMFLOAT3 add_angle{};
		DirectX::XMFLOAT3 add_translate{};
		DirectX::XMFLOAT3 blend_add_angle{};
		DirectX::XMFLOAT3 blend_add_translate{};

		//���[�V�����u�����h�p�ϐ�
		bool is_blend = false;
		int blend_type = BlendType::First;

		std::vector<Node*>	children;


	};



	// �s��v�Z
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// �m�[�h���X�g�擾
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	// ���\�[�X�擾
	const ModelResource* GetResource() const { return resource.get(); }

	//�A�j���[�V�����X�V����
	void UpdateAnimation(float elapsedTime);

	//�A�j���[�V�����Đ�
	void PlayAnimation(int index, bool loop, float blenSeconds = 0.2f);

	//�A�j���[�V�����Đ�����
	bool IsPlayAnimation() const;

	//�Đ����Ԏ擾
	float GetCurrentAnimationSeconds() { return currentAnimationSeconds; }
	//�Đ����ԃZ�b�g
	void SetCurrentAnimationSeconds(float time) { currentAnimationSeconds = time; }

	int GetAnimationIndex() { return currentAnimationIndex; }
	void SetAnimationIndex(int in) { currentAnimationIndex = in; }

	//�A�j���[�V�������x�ݒ�
	void SetAnimationSpeed(float speed) { animation_speed = speed; }

	void SetAnimationEndFlag(bool is) { animationEndFlag = is; }

	//////////////////////////////////////////
	///////     ���[�V�����u�����h�֌W     /////
	//////////////////////////////////////////

	//�A�j���[�V�����X�V����
	void UpdateMotionBlend(float elapsedTime);

	//�A�j���[�V�����Đ�
	void BlendPlayAnimation(int index, bool loop, int BIndex, bool BLoop, float blenSeconds = 0.2f, float blend_blenSeconds = 0.2f);

	//�A�j���[�V�����Đ�����
	bool IsPlayBlendsAnimation() const;

	//�Đ����Ԏ擾
	float GetBlendsCurrentAnimationSeconds() { return BlendsCurrentAnimationSeconds; }
	//�Đ����ԃZ�b�g
	void SetBlendsCurrentAnimationSeconds(float time) { BlendsCurrentAnimationSeconds = time; }

	//���[�V�����u�����h����
	bool GetIsMotionBlend() { return isMotionBlend; }

	//���[�V�����u�����h�ݒ�
	void BlendSetting(const char* name, bool is = true, bool my_only = true);
	void BlendSetting(const char* name, int type, bool my_only = true);
	void BlendReset();

	void SetBlendAnimationIndex(int in) { BlendsCurrentAnimationIndex = in; }

	//�A�j���[�V�������x�ݒ�
	void SetBlendAnimationSpeed(float speed) { blend_animation_speed = speed; }

	void SetBlendAnimationEndFlag(bool is) { BlendAnimationEndFlag = is; }

	//////////////////////////////////////////

	//�m�[�h����
	Node* FindNode(const char* name);
	bool FindNode(Node node, const char* name);

	//�e�m�[�h����
	Node* FindParentNode(const char* name);
	bool FindParentNode(Node node, const char* name);

	//////////////////   �V�F�[�_�[�p   ////////////////////////
	virtual void SetRenderContext(RenderContext* rc) {}
	
	//////////////////////////////////////////

private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;

	int currentAnimationIndex = -1;
	float currentAnimationSeconds = 0.0f;
	bool animationLoopFlag = false;
	bool animationEndFlag = false;
	float animetionBlendTime = 0.0f;
	float animetionBlendSeconds = 0.0f;
	float animation_speed = 1.0f;

	//���[�V�����u�����h
	bool isMotionBlend = false;//���[�V�����u�����h����
	int BlendsCurrentAnimationIndex = -1;
	float BlendsCurrentAnimationSeconds = 0.0f;
	bool BlendAnimationLoopFlag = false;
	bool BlendAnimationEndFlag = false;
	float blend_animetionBlendTime = 0.0f;
	float blend_animetionBlendSeconds = 0.0f;
	float blend_animation_speed = 1.0f;
};
