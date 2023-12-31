#pragma once

//#include "BaseRenderer.h"
#include "Graphics/Model/Model.h"

class ModelRenderer
{
public:
	ModelRenderer() {}
	~ModelRenderer() {}

	//	描画モデルの登録
	void RegisterRenderModel(Model* model);

	//	描画モデルの登録解除
	void UnregisterRenderModel(Model* model);

	//	描画モデル登録全解除
	void ClearRenderModel();

	//  描画モデル数取得
	int GetRenderModelCount() const { return static_cast<int>(render_models.size()); }
	
	//  描画モデル取得
	Model* GetRenderModel(int index) { return render_models.at(index); }

	//  描画モデル数取得
	int GetModelCount() const { return static_cast<int>(render_models.size()); }

	// 描画モデルリスト取得
	std::vector<Model*> GetRenderModels() { return render_models; }
private:
	std::vector<Model*>	render_models; // 描画モデルリスト
};
