#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"
#include "ResourceManager.h"

// �R���X�g���N�^
Model::Model(const char* filename)
{
	// ���\�[�X�ǂݍ���
	resource = ResourceManager::Instance().LoadModelResource(filename);

	// �m�[�h
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());

	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		dst.basic_scale = src.scale;
		dst.basic_rotate = src.rotate;
		dst.basic_translate = src.translate;

		dst.blend_scale = src.scale;
		dst.blend_rotate = src.rotate;
		dst.blend_translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// �s��v�Z
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// �ϊ��s��v�Z
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
	for (Node& node : nodes)
	{
		// ���[���h�s��Z�o
		DirectX::XMMATRIX ParentTransform;
		DirectX::XMMATRIX S;
		DirectX::XMMATRIX R;
		DirectX::XMMATRIX T;
		DirectX::XMMATRIX LocalTransform;
		DirectX::XMMATRIX WorldTransform;

		// �X�P�[���s����쐬
		S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		//�ʒu�s����쐬
		T = DirectX::XMMatrixTranslation(node.add_translate.x, node.add_translate.y, node.add_translate.z);

		//��]�s����쐬
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(node.add_angle.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(node.add_angle.y);
		DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(node.add_angle.z);
		R = Y * X * Z;

		//�R�̍s���g�ݍ��킹�A���[���h�s����쐬
		DirectX::XMMATRIX AddWorldTransform = S * R * T;

		T = DirectX::XMMatrixTranslation(node.blend_add_translate.x, node.blend_add_translate.y, node.blend_add_translate.z);
		//��]�s����쐬
		X = DirectX::XMMatrixRotationX(node.blend_add_angle.x);
		Y = DirectX::XMMatrixRotationY(node.blend_add_angle.y);
		Z = DirectX::XMMatrixRotationZ(node.blend_add_angle.z);
		R = Y * X * Z;
		DirectX::XMMATRIX AddBlendWorldTransform = S * R * T;

		switch (node.blend_type)
		{
		case BlendType::First:
			// ���[�J���s��Z�o
			S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
			R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
			T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
			LocalTransform = S * R * T;

			if (node.parent != nullptr)
			{
				ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
			}
			else
			{
				ParentTransform = Transform;
			}
			WorldTransform = AddWorldTransform * LocalTransform * ParentTransform;

			// �v�Z���ʂ��i�[
			DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);

			DirectX::XMStoreFloat4x4(&node.blend_localTransform, LocalTransform);
			DirectX::XMStoreFloat4x4(&node.blend_worldTransform, WorldTransform);
			break;
		case BlendType::Second:
			// ���[�J���s��Z�o
			S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
			R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
			T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
			LocalTransform = S * R * T;

			if (node.parent != nullptr)
			{
				ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->blend_worldTransform);
			}
			else
			{
				ParentTransform = Transform;
			}
			WorldTransform = AddBlendWorldTransform * LocalTransform * ParentTransform;

			// �v�Z���ʂ��i�[
			DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);

			DirectX::XMStoreFloat4x4(&node.blend_localTransform, LocalTransform);
			DirectX::XMStoreFloat4x4(&node.blend_worldTransform, WorldTransform);
			break;
		case BlendType::Full:
			// ���[�J���s��Z�o
			S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
			R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
			T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
			LocalTransform = S * R * T;

			S = DirectX::XMMatrixScaling(node.blend_scale.x, node.blend_scale.y, node.blend_scale.z);
			R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.blend_rotate));
			T = DirectX::XMMatrixTranslation(node.blend_translate.x, node.blend_translate.y, node.blend_translate.z);
			DirectX::XMMATRIX LocalTransform2 = S * R * T;

			DirectX::XMMATRIX ParentTransform2;
			if (node.parent != nullptr)
			{
				ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
				ParentTransform2 = DirectX::XMLoadFloat4x4(&node.parent->blend_worldTransform);
			}
			else
			{
				ParentTransform = Transform;
				ParentTransform2 = Transform;
			}
			WorldTransform = AddWorldTransform * LocalTransform * ParentTransform;
			DirectX::XMMATRIX WorldTransform2 = AddBlendWorldTransform * LocalTransform2 * ParentTransform2;

			// �v�Z���ʂ��i�[
			DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);

			DirectX::XMStoreFloat4x4(&node.blend_localTransform, LocalTransform2);
			DirectX::XMStoreFloat4x4(&node.blend_worldTransform, WorldTransform2);
			break;
		}
	}
}

//�A�j���[�V�����X�V����
void Model::UpdateAnimation(float elapsedTime)
{
	if (isMotionBlend)
	{
		//���[�V�����u�����h���Ȃ烂�[�V�����u�����h�p�̍X�V�������Ăяo��
		UpdateMotionBlend(elapsedTime);
		return;
	}

	//�Đ����łȂ��Ȃ珈�����Ȃ�
	if (!IsPlayAnimation())return;

	//�u�����h���̌v�Z
	float blendRate = 1.0f;
	animetionBlendTime += elapsedTime;
	blendRate = animetionBlendTime / animetionBlendSeconds;

	//�w��̃A�j���[�V�����f�[�^���擾
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	//�A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		//���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentAnimationSeconds >= keyframe0.seconds &&
			currentAnimationSeconds < keyframe1.seconds)
		{
			//�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
			float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				//2�̃L�[�t���[���Ԃ̕⊮�v�Z
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR key0_pos = DirectX::XMLoadFloat3(&key0.translate);
				DirectX::XMVECTOR key1_pos = DirectX::XMLoadFloat3(&key1.translate);
				DirectX::XMVECTOR key0_rot = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR key1_rot = DirectX::XMLoadFloat4(&key1.rotate);

				DirectX::XMVECTOR Position = DirectX::XMVectorLerp(key0_pos, key1_pos, rate);
				DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(key0_rot, key1_rot, rate);

				if (blendRate < 1.0f)
				{
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&node.translate);
					DirectX::XMVECTOR rot = DirectX::XMLoadFloat4(&node.rotate);

					DirectX::XMVECTOR Pos = DirectX::XMVectorLerp(pos, Position, blendRate);
					DirectX::XMVECTOR Rot = DirectX::XMQuaternionSlerp(rot, Rotation, blendRate);

					DirectX::XMStoreFloat3(&node.translate, Pos);
					DirectX::XMStoreFloat4(&node.rotate, Rot);

					DirectX::XMStoreFloat3(&node.blend_translate, Pos);
					DirectX::XMStoreFloat4(&node.blend_rotate, Rot);

#if MOVE_ANGLE
					//���̕���
					if (node.is_blend)
					{
						//�p�x����
						DirectX::XMVECTOR ADD;

						DirectX::XMFLOAT3 up = { 0,0,1 };
						DirectX::XMVECTOR UP = DirectX::XMLoadFloat3(&up);
						ADD = DirectX::XMQuaternionRotationAxis(UP, add_angle);
						ADD = DirectX::XMVector4Normalize(ADD);
						DirectX::XMFLOAT4 uu;
						DirectX::XMStoreFloat4(&uu, ADD);
						node.rotate.x += uu.x;
						node.rotate.y += uu.y;
						node.rotate.z += uu.z;
						node.blend_rotate.x += uu.x;
						node.blend_rotate.y += uu.y;
						node.blend_rotate.z += uu.z;

						DirectX::XMFLOAT4 n = { node.rotate.x, node.rotate.y ,node.rotate.z,node.rotate.w };
						DirectX::XMVECTOR N = DirectX::XMLoadFloat4(&n);
						N = DirectX::XMVector4Normalize(N);
						DirectX::XMStoreFloat4(&n, N);
						node.rotate.x = n.x;
						node.rotate.y = n.y;
						node.rotate.z = n.z;
						node.rotate.w = n.w;

						n = { node.blend_rotate.x, node.blend_rotate.y ,node.blend_rotate.z,node.blend_rotate.w };
						N = DirectX::XMLoadFloat4(&n);
						N = DirectX::XMVector4Normalize(N);
						DirectX::XMStoreFloat4(&n, N);
						node.blend_rotate.x = n.x;
						node.blend_rotate.y = n.y;
						node.blend_rotate.z = n.z;
						node.blend_rotate.z = n.z;

						//�ʒu����
						DirectX::XMFLOAT3 zero = { 0,0,0 };
						DirectX::XMVECTOR ZERO = DirectX::XMLoadFloat3(&zero);
						DirectX::XMVECTOR SUB = DirectX::XMVectorSubtract(ZERO, Pos);
						DirectX::XMVECTOR LEN = DirectX::XMVector3Length(SUB);
						float len;
						DirectX::XMStoreFloat(&len, LEN);
						float an = atan2f(node.translate.z, node.translate.x);// -DirectX::XMConvertToRadians(90);
						an += add_angle;
						node.translate.x = cosf(an) * len;
						node.translate.z = sinf(an) * len;

						SUB = DirectX::XMVectorSubtract(ZERO, Pos);
						LEN = DirectX::XMVector3Length(SUB);
						DirectX::XMStoreFloat(&len, LEN);
						an = atan2f(node.blend_translate.z, node.blend_translate.x);// -DirectX::XMConvertToRadians(90);
						an += add_angle;
						node.blend_translate.x = cosf(an) * len;
						node.blend_translate.z = sinf(an) * len;
					}
#endif
				}
				else
				{
					DirectX::XMStoreFloat3(&node.translate, Position);
					DirectX::XMStoreFloat4(&node.rotate, Rotation);

					DirectX::XMStoreFloat3(&node.blend_translate, Position);
					DirectX::XMStoreFloat4(&node.blend_rotate, Rotation);

#if MOVE_ANGLE
					//���̕���
					if (node.is_blend)
					{
						//�p�x����
						DirectX::XMVECTOR ADD;

						DirectX::XMFLOAT3 up = { 0,0,1 };
						DirectX::XMVECTOR UP = DirectX::XMLoadFloat3(&up);
						ADD = DirectX::XMQuaternionRotationAxis(UP, add_angle);
						ADD = DirectX::XMVector4Normalize(ADD);
						DirectX::XMStoreFloat3(&up, ADD);
						node.rotate.x += up.x;
						node.rotate.y += up.y;
						node.rotate.z += up.z;
						node.blend_rotate.x += up.x;
						node.blend_rotate.y += up.y;
						node.blend_rotate.z += up.z;

						DirectX::XMFLOAT4 n = { node.rotate.x, node.rotate.y ,node.rotate.z,node.rotate.w };
						DirectX::XMVECTOR N = DirectX::XMLoadFloat4(&n);
						N = DirectX::XMVector4Normalize(N);
						DirectX::XMStoreFloat4(&n, N);
						node.rotate.x = n.x;
						node.rotate.y = n.y;
						node.rotate.z = n.z;
						node.rotate.w = n.w;

						n = { node.blend_rotate.x, node.blend_rotate.y ,node.blend_rotate.z ,node.blend_rotate.w };
						N = DirectX::XMLoadFloat4(&n);
						N = DirectX::XMVector4Normalize(N);
						DirectX::XMStoreFloat4(&n, N);
						node.blend_rotate.x = n.x;
						node.blend_rotate.y = n.y;
						node.blend_rotate.z = n.z;
						node.blend_rotate.w = n.w;


						//�ʒu����
						DirectX::XMFLOAT3 zero = { 0,0,0 };
						DirectX::XMVECTOR ZERO = DirectX::XMLoadFloat3(&zero);
						DirectX::XMVECTOR SUB = DirectX::XMVectorSubtract(ZERO, Position);
						DirectX::XMVECTOR LEN = DirectX::XMVector3Length(SUB);
						float len;
						DirectX::XMStoreFloat(&len, LEN);
						float an = atan2f(node.translate.z, node.translate.x);// -DirectX::XMConvertToRadians(90);
						an += add_angle;
						node.translate.x = cosf(an) * len;
						node.translate.z = sinf(an) * len;

						SUB = DirectX::XMVectorSubtract(ZERO, Position);
						LEN = DirectX::XMVector3Length(SUB);
						DirectX::XMStoreFloat(&len, LEN);
						an = atan2f(node.blend_translate.z, node.blend_translate.x);// -DirectX::XMConvertToRadians(90);
						an += add_angle;
						node.blend_translate.x = cosf(an) * len;
						node.blend_translate.z = sinf(an) * len;
					}
#endif
				}
			}
			break;
		}
	}

	//�ŏI�t���[������
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}


	//���Ԍo��
	currentAnimationSeconds += elapsedTime * animation_speed;

	//�Đ����Ԃ��I�[���Ԃ𒴂�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			//�Đ����Ԃ������߂�
			currentAnimationSeconds -= animation.secondsLength;
		}
		else { animationEndFlag = true; }
	}

	//�t�Đ�Ver
	if (currentAnimationSeconds < 0.0f)
	{
		if (animationLoopFlag)
		{
			//�Đ����Ԃ������߂�
			currentAnimationSeconds += animation.secondsLength;
		}
		else { animationEndFlag = true; }
	}
}

//�A�j���[�V�����Đ�
void Model::PlayAnimation(int index, bool loop, float blenSeconds)
{
	isMotionBlend = false;

	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag = false;

	animetionBlendTime = 0.0f;
	animetionBlendSeconds = (std::max)(0.001f, blenSeconds);

	animation_speed = 1.0f;
}

//�A�j���[�V�����Đ�����
bool Model::IsPlayAnimation() const
{
	if (currentAnimationIndex < 0)return false;
	if (currentAnimationIndex >= resource->GetAnimations().size())return false;
	return true;
}

//�u�����h�p�A�j���[�V�����X�V����
void Model::UpdateMotionBlend(float elapsedTime)
{
	//�u�����h���̌v�Z
	float blendRate = 1.0f;
	animetionBlendTime += elapsedTime;
	blendRate = animetionBlendTime / animetionBlendSeconds;
	//���[�V�����u�����h�p
	float blend_blendRate = 1.0f;
	blend_animetionBlendTime += elapsedTime;
	blend_blendRate = blend_animetionBlendTime / blend_animetionBlendSeconds;


	//�w��̃A�j���[�V�����f�[�^���擾
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	int nodeCount = static_cast<int>(nodes.size());

	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		Node& node = nodes[nodeIndex];
		if (node.blend_type == BlendType::First ||
			node.blend_type == BlendType::Full)
		{
			//�Đ����łȂ��Ȃ珈�����Ȃ�
			if (IsPlayAnimation())
			{
				const ModelResource::Animation& animation = animations.at(currentAnimationIndex);
				const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
				int keyCount = static_cast<int>(keyframes.size());
				for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
				{
					//���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
					const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
					const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
					if ((currentAnimationSeconds >= keyframe0.seconds &&
						currentAnimationSeconds < keyframe1.seconds) ||
						keyIndex == keyCount - 2)
					{
						//�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
						float rate = (std::min)(1.0f, (currentAnimationSeconds - keyframe0.seconds) /
							(keyframe1.seconds - keyframe0.seconds));

						//2�̃L�[�t���[���Ԃ̕⊮�v�Z
						const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
						const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

						Node& node = nodes[nodeIndex];

						DirectX::XMVECTOR key0_pos = DirectX::XMLoadFloat3(&key0.translate);
						DirectX::XMVECTOR key1_pos = DirectX::XMLoadFloat3(&key1.translate);
						DirectX::XMVECTOR key0_rot = DirectX::XMLoadFloat4(&key0.rotate);
						DirectX::XMVECTOR key1_rot = DirectX::XMLoadFloat4(&key1.rotate);

						DirectX::XMVECTOR key0_sca = DirectX::XMLoadFloat3(&key0.scale);
						DirectX::XMVECTOR key1_sca = DirectX::XMLoadFloat3(&key1.scale);

						DirectX::XMVECTOR Position = DirectX::XMVectorLerp(key0_pos, key1_pos, rate);
						DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(key0_sca, key1_sca, rate);
						DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(key0_rot, key1_rot, rate);

						if (blendRate < 1.0f)
						{
							DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&node.translate);
							DirectX::XMVECTOR rot = DirectX::XMLoadFloat4(&node.rotate);
							DirectX::XMVECTOR sca = DirectX::XMLoadFloat3(&node.scale);

							DirectX::XMVECTOR Pos = DirectX::XMVectorLerp(pos, Position, blendRate);
							DirectX::XMVECTOR Rot = DirectX::XMQuaternionSlerp(rot, Rotation, blendRate);
							DirectX::XMVECTOR Sca = DirectX::XMVectorLerp(sca, Scale, blendRate);

							DirectX::XMStoreFloat3(&node.translate, Pos);
							DirectX::XMStoreFloat4(&node.rotate, Rot);
						}
						else
						{
							DirectX::XMStoreFloat3(&node.translate, Position);
							DirectX::XMStoreFloat4(&node.rotate, Rotation);
						}
						break;
					}
				}
			}
		}
	}

	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		Node& node = nodes[nodeIndex];
		if (node.blend_type == BlendType::Second ||
			node.blend_type == BlendType::Full)
		{
			//�����𖞂����Ă���΃��[�V�����u�����h�̃A�j���[�V�������Đ�
			//�Đ����łȂ��Ȃ珈�����Ȃ�
			if (IsPlayBlendsAnimation())
			{
				const ModelResource::Animation& MBAnimation = animations.at(BlendsCurrentAnimationIndex);
				const std::vector<ModelResource::Keyframe>& keyframes = MBAnimation.keyframes;
				int keyCount = static_cast<int>(keyframes.size());
				for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
				{
					//���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
					const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
					const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
					if ((BlendsCurrentAnimationSeconds >= keyframe0.seconds &&
						BlendsCurrentAnimationSeconds < keyframe1.seconds) ||
						keyIndex == keyCount - 2)
					{
						//�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
						float rate = (std::min)(1.0f, (BlendsCurrentAnimationSeconds - keyframe0.seconds) /
							(keyframe1.seconds - keyframe0.seconds));

						//2�̃L�[�t���[���Ԃ̕⊮�v�Z
						const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
						const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);


						DirectX::XMVECTOR key0_pos = DirectX::XMLoadFloat3(&key0.translate);
						DirectX::XMVECTOR key1_pos = DirectX::XMLoadFloat3(&key1.translate);
						DirectX::XMVECTOR key0_rot = DirectX::XMLoadFloat4(&key0.rotate);
						DirectX::XMVECTOR key1_rot = DirectX::XMLoadFloat4(&key1.rotate);

						DirectX::XMVECTOR key0_sca = DirectX::XMLoadFloat3(&key0.scale);
						DirectX::XMVECTOR key1_sca = DirectX::XMLoadFloat3(&key1.scale);

						DirectX::XMVECTOR Position = DirectX::XMVectorLerp(key0_pos, key1_pos, rate);
						DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(key0_sca, key1_sca, rate);
						DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(key0_rot, key1_rot, rate);

						if (blend_blendRate < 1.0f)
						{
							DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&node.translate);
							DirectX::XMVECTOR rot = DirectX::XMLoadFloat4(&node.rotate);
							DirectX::XMVECTOR sca = DirectX::XMLoadFloat3(&node.scale);

							DirectX::XMVECTOR Pos = DirectX::XMVectorLerp(pos, Position, blend_blendRate);
							DirectX::XMVECTOR Rot = DirectX::XMQuaternionSlerp(rot, Rotation, blend_blendRate);
							DirectX::XMVECTOR Sca = DirectX::XMVectorLerp(sca, Scale, blend_blendRate);

							if (node.blend_type == BlendType::Second)
							{
								DirectX::XMStoreFloat3(&node.translate, Pos);
								DirectX::XMStoreFloat4(&node.rotate, Rot);
							}
							else
							{
								DirectX::XMStoreFloat3(&node.blend_translate, Pos);
								DirectX::XMStoreFloat4(&node.blend_rotate, Rot);
							}
						}
						else
						{
							if (node.blend_type == BlendType::Second)
							{
								DirectX::XMStoreFloat3(&node.translate, Position);
								DirectX::XMStoreFloat4(&node.rotate, Rotation);
								//DirectX::XMStoreFloat3(&node.scale, Scale);
							}
							else
							{
								DirectX::XMStoreFloat3(&node.blend_translate, Position);
								DirectX::XMStoreFloat4(&node.blend_rotate, Rotation);
							}
						}
						break;
					}
				}
			}
		}
	}
	//�ŏI�t���[������
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
	}
	//�u�����h�p
	if (BlendAnimationEndFlag)
	{
		BlendAnimationEndFlag = false;
		BlendsCurrentAnimationIndex = -1;
	}
	//���Ԍo��
	currentAnimationSeconds += elapsedTime * animation_speed;
	BlendsCurrentAnimationSeconds += elapsedTime * blend_animation_speed;

	//�Đ����Ԃ��I�[���Ԃ𒴂�����
	if (IsPlayAnimation())
	{
		const ModelResource::Animation& animation = animations.at(currentAnimationIndex);
		if (currentAnimationSeconds >= animation.secondsLength)
		{
			if (animationLoopFlag)
			{
				//�Đ����Ԃ������߂�
				currentAnimationSeconds -= animation.secondsLength;
			}
			else { animationEndFlag = true; }
		}

		//�t�Đ�Ver
		if (currentAnimationSeconds < 0.0f)
		{
			if (animationLoopFlag)
			{
				//�Đ����Ԃ������߂�
				currentAnimationSeconds += animation.secondsLength;
			}
			else { animationEndFlag = true; }
		}
	}

	//�u�����h�A�j���[�V�����p
	if (IsPlayBlendsAnimation())
	{
		const ModelResource::Animation& MBAnimation = animations.at(BlendsCurrentAnimationIndex);
		if (BlendsCurrentAnimationSeconds >= MBAnimation.secondsLength)
		{
			if (BlendAnimationLoopFlag)
			{
				//�Đ����Ԃ������߂�
				BlendsCurrentAnimationSeconds -= MBAnimation.secondsLength;
			}
			else { BlendAnimationEndFlag = true; }
		}

		//�t�Đ�Ver
		if (BlendsCurrentAnimationSeconds < 0.0f)
		{
			if (BlendAnimationLoopFlag)
			{
				//�Đ����Ԃ������߂�
				BlendsCurrentAnimationSeconds += MBAnimation.secondsLength;
			}
			else { BlendAnimationEndFlag = true; }
		}
	}
}

//�u�����h�p�A�j���[�V�����Đ�
void Model::BlendPlayAnimation(int index, bool loop, int MBIndex, bool MBLoop, float blenSeconds, float blend_blenSeconds)
{
	isMotionBlend = true;

	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animation_speed = 1.0f;

	BlendsCurrentAnimationIndex = MBIndex;
	BlendsCurrentAnimationSeconds = 0.0f;
	BlendAnimationLoopFlag = MBLoop;
	BlendAnimationEndFlag = false;
	blend_animation_speed = 1.0f;

	animetionBlendTime = 0.0f;
	animetionBlendSeconds = (std::max)(0.001f, blenSeconds);

	blend_animetionBlendTime = 0.0f;
	blend_animetionBlendSeconds = (std::max)(0.001f, blend_blenSeconds);
}

//�u�����h�p�A�j���[�V�����Đ�����
bool Model::IsPlayBlendsAnimation() const
{
	if (BlendsCurrentAnimationIndex < 0)return false;
	if (BlendsCurrentAnimationIndex >= resource->GetAnimations().size())return false;
	return true;
}

//���[�V�����u�����h�ݒ�
void Model::BlendSetting(const char* name, bool is, bool my_only)
{
	for (Node& node : nodes)
	{
		if (my_only)
		{
			if (FindNode(node, name))
			{
				node.is_blend = is;
			}
		}
		else
		{
			if (FindParentNode(node, name))
			{
				node.is_blend = is;
			}
		}
	}
}

void Model::BlendSetting(const char* name, int type, bool my_only)
{
	for (Node& node : nodes)
	{
		if (my_only)
		{
			if (FindNode(node, name))
			{
				node.blend_type = type;
			}
		}
		else
		{
			if (FindParentNode(node, name))
			{
				node.blend_type = type;
			}
		}
	}
}

void Model::BlendReset()
{
	for (Node& node : nodes)
	{
		node.is_blend = false;
	}
}

//�m�[�h����
Model::Node* Model::FindNode(const char* name)
{
	for (Node& node : nodes)
	{
		if (strcmp(node.name, name) == 0)
		{
			return &node;
		}
	}

	//������Ȃ�����
	return nullptr;
}

bool Model::FindNode(Node node, const char* name)
{
	if (strcmp(node.name, name) == 0)
	{
		return true;
	}
	return false;
}

//�e�m�[�h����
Model::Node* Model::FindParentNode(const char* name)
{
	for (Node& node : nodes)
	{
		Node* pa = node.parent;
		while (pa != nullptr)
		{
			if (strcmp(pa->name, name) == 0)
			{
				return &node;
			}
			pa = pa->parent;
		}
	}

	//������Ȃ�����
	return nullptr;
}

bool Model::FindParentNode(Node node, const char* name)
{
	Node* pa = &node;
	while (pa != nullptr)
	{
		if (pa == nullptr)return false;
		bool is = pa == nullptr;

		if (strcmp(pa->name, name) == 0)
		{
			return true;
		}
		pa = pa->parent;
	}
	return false;
}