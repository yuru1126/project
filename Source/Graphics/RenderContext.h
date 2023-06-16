#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

///////////     �V�F�[�_�[�֘A     //////////////////
//�{���m�C�V�F�[�_�[�p�f�[�^
struct VoronoiData
{
	//x : X�̕�  y : Y�̕�  z : X�̃Y��  w : �o�ߎ���
	DirectX::XMFLOAT4		lenge;
	//���Ԍo�߂̔����p
	DirectX::XMFLOAT4		time_line;
};

//�|���S���V�F�[�_�[�p�f�[�^
struct PolygonData
{
	float time;
	float destruction_factor;
	float position_factor;
	float rotation_factor;
	float scale_factor;
	float yplas_factor;
	float multiply;

	int geometry_type;

	//Y�����̕�Ԓl
	DirectX::XMFLOAT2 interpolation_vertical;
	
	//Y�̈ʒu����ʐς̑傫����ύX����
	DirectX::XMFLOAT2 area_size_vertical;
	
	//���f����Y�̍ő�l�̏�����
	float vertical_end;
	
	//���f���̃|�W�V����
	DirectX::XMFLOAT3 model_position;
};

enum Geometr_type
{
	//���S��
	DEATH,
	//�o�ꎞ
	BIRTH,
	DEATH_NOT_TIMER = 3,
	BIRTH_NOT_TIMER 
};


//���e�e�N�X�`���p�f�[�^
struct ProjectionTextureData
{
	DirectX::XMFLOAT4X4 circle_transform;
	int CircleFlag;
	DirectX::XMFLOAT3 dummy;
};


////////////////////////////////////////////////////

// �����_�[�R���e�L�X�g
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	
	//xy : �X�N���[���̃T�C�Y, z : near�N���b�v�����Aw : far�N���b�v����
	DirectX::XMFLOAT4			screenSize;	

	//���C�g���
	DirectX::XMFLOAT4		lightDirection;
	DirectX::XMFLOAT4		ambientLightColor;				// �������

	//�J�������
	DirectX::XMFLOAT4		viewPosition;// camera_position
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;


	//�{���m�C�V�F�[�_�[�p�f�[�^
	VoronoiData voronoi_data;

	//�|���S���V�F�[�_�[�p�f�[�^
	PolygonData polygon_data;

	//���e�e�N�X�`���p�f�[�^
	ProjectionTextureData projection_texture_data;
};
