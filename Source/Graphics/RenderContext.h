#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

///////////     シェーダー関連     //////////////////
//ボロノイシェーダー用データ
struct VoronoiData
{
	//x : Xの幅  y : Yの幅  z : Xのズレ  w : 経過時間
	DirectX::XMFLOAT4		lenge;
	//時間経過の白線用
	DirectX::XMFLOAT4		time_line;
};

//ポリゴンシェーダー用データ
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

	//Y方向の補間値
	DirectX::XMFLOAT2 interpolation_vertical;
	
	//Yの位置から面積の大きさを変更する
	DirectX::XMFLOAT2 area_size_vertical;
	
	//モデルのYの最大値の少し上
	float vertical_end;
	
	//モデルのポジション
	DirectX::XMFLOAT3 model_position;
};

enum Geometr_type
{
	//死亡時
	DEATH,
	//登場時
	BIRTH,
	DEATH_NOT_TIMER = 3,
	BIRTH_NOT_TIMER 
};


//投影テクスチャ用データ
struct ProjectionTextureData
{
	DirectX::XMFLOAT4X4 circle_transform;
	int CircleFlag;
	DirectX::XMFLOAT3 dummy;
};


////////////////////////////////////////////////////

// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	
	//xy : スクリーンのサイズ, z : nearクリップ距離、w : farクリップ距離
	DirectX::XMFLOAT4			screenSize;	

	//ライト情報
	DirectX::XMFLOAT4		lightDirection;
	DirectX::XMFLOAT4		ambientLightColor;				// 環境光情報

	//カメラ情報
	DirectX::XMFLOAT4		viewPosition;// camera_position
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;


	//ボロノイシェーダー用データ
	VoronoiData voronoi_data;

	//ポリゴンシェーダー用データ
	PolygonData polygon_data;

	//投影テクスチャ用データ
	ProjectionTextureData projection_texture_data;
};
