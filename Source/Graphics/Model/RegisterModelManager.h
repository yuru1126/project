#pragma once
#include <vector>
#include <memory>

#include "Graphics/Model/Model.h"
#include "Renderer/ModelRenderer.h"

//モデル登録用マネージャー
class RegisterModelManager
{
    RegisterModelManager() {};
    ~RegisterModelManager() {};
public:
	static RegisterModelManager& Instance()
	{
		static	RegisterModelManager	instance;
		return	instance;
	}

	//////    モデル登録,解除関連    //////
	void RegisterRenderModel(ModelRenderer* mr);
	void UnregisterRenderModel(ModelRenderer* mr);

	void RegisterModel(Model* model);
	void ExclusionModel(Model* model);


	//////    ボロノイ用    //////
	void VoronoiRegisterRenderModel(ModelRenderer* mr);
	void VoronoiUnregisterRenderModel(ModelRenderer* mr);
	
	void VoronoiRegisterModel(Model* model);
	void VoronoiExclusionModel(Model* model);


	///////    ポリゴン用    //////
	void PolygonRegisterRenderModel(ModelRenderer* mr);
	void PolygonUnregisterRenderModel(ModelRenderer* mr);
	
	void PolygonRegisterModel(Model* model);
	void PolygonExclusionModel(Model* model);


	//////		全て同じ場合    //////
	void AllRegisterRenderModel(ModelRenderer* mr);
	void AllUnregisterRenderModel(ModelRenderer* mr);

	//モデル削除
	void Clear();
	void VoronoiClear();
	void PolygonClear();
	void AllClear();


private:
	//モデル登録
	std::vector<Model*>     register_model;
	std::vector<Model*>     voronoi_register_model;//ボロノイ用
	std::vector<Model*>     polygon_register_model;//ポリゴン用


	//モデル解除
	std::vector<Model*>     exclusion_model;
	std::vector<Model*>     voronoi_exclusion_model;//ボロノイ用
	std::vector<Model*>     polygon_exclusion_model;//ポリゴン用

};