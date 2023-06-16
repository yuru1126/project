#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics/Model/ModelResource.h"
#include "Graphics/RenderContext.h"

//
#define MOVE_ANGLE FALS

// モデル
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

		//歩き等のブレンド用
		//元(基本)の位置
		DirectX::XMFLOAT3	basic_scale{};
		DirectX::XMFLOAT4	basic_rotate{};
		DirectX::XMFLOAT3	basic_translate{};

		//分離用(仮)Divide
		DirectX::XMFLOAT3	blend_scale{};
		DirectX::XMFLOAT4	blend_rotate{};
		DirectX::XMFLOAT3	blend_translate{};
		DirectX::XMFLOAT4X4	blend_localTransform{};
		DirectX::XMFLOAT4X4	blend_worldTransform{};


		DirectX::XMFLOAT3 add_angle{};
		DirectX::XMFLOAT3 add_translate{};
		DirectX::XMFLOAT3 blend_add_angle{};
		DirectX::XMFLOAT3 blend_add_translate{};

		//モーションブレンド用変数
		bool is_blend = false;
		int blend_type = BlendType::First;

		std::vector<Node*>	children;


	};



	// 行列計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	//アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

	//アニメーション再生
	void PlayAnimation(int index, bool loop, float blenSeconds = 0.2f);

	//アニメーション再生中か
	bool IsPlayAnimation() const;

	//再生時間取得
	float GetCurrentAnimationSeconds() { return currentAnimationSeconds; }
	//再生時間セット
	void SetCurrentAnimationSeconds(float time) { currentAnimationSeconds = time; }

	int GetAnimationIndex() { return currentAnimationIndex; }
	void SetAnimationIndex(int in) { currentAnimationIndex = in; }

	//アニメーション速度設定
	void SetAnimationSpeed(float speed) { animation_speed = speed; }

	void SetAnimationEndFlag(bool is) { animationEndFlag = is; }

	//////////////////////////////////////////
	///////     モーションブレンド関係     /////
	//////////////////////////////////////////

	//アニメーション更新処理
	void UpdateMotionBlend(float elapsedTime);

	//アニメーション再生
	void BlendPlayAnimation(int index, bool loop, int BIndex, bool BLoop, float blenSeconds = 0.2f, float blend_blenSeconds = 0.2f);

	//アニメーション再生中か
	bool IsPlayBlendsAnimation() const;

	//再生時間取得
	float GetBlendsCurrentAnimationSeconds() { return BlendsCurrentAnimationSeconds; }
	//再生時間セット
	void SetBlendsCurrentAnimationSeconds(float time) { BlendsCurrentAnimationSeconds = time; }

	//モーションブレンド中か
	bool GetIsMotionBlend() { return isMotionBlend; }

	//モーションブレンド設定
	void BlendSetting(const char* name, bool is = true, bool my_only = true);
	void BlendSetting(const char* name, int type, bool my_only = true);
	void BlendReset();

	void SetBlendAnimationIndex(int in) { BlendsCurrentAnimationIndex = in; }

	//アニメーション速度設定
	void SetBlendAnimationSpeed(float speed) { blend_animation_speed = speed; }

	void SetBlendAnimationEndFlag(bool is) { BlendAnimationEndFlag = is; }

	//////////////////////////////////////////

	//ノード検索
	Node* FindNode(const char* name);
	bool FindNode(Node node, const char* name);

	//親ノード検索
	Node* FindParentNode(const char* name);
	bool FindParentNode(Node node, const char* name);

	//////////////////   シェーダー用   ////////////////////////
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

	//モーションブレンド
	bool isMotionBlend = false;//モーションブレンド中か
	int BlendsCurrentAnimationIndex = -1;
	float BlendsCurrentAnimationSeconds = 0.0f;
	bool BlendAnimationLoopFlag = false;
	bool BlendAnimationEndFlag = false;
	float blend_animetionBlendTime = 0.0f;
	float blend_animetionBlendSeconds = 0.0f;
	float blend_animation_speed = 1.0f;
};
