#include "Renderer/ModelRenderer.h"

//	描画モデルの登録
void ModelRenderer::RegisterRenderModel(Model* model)
{

	// モデルが既に登録されているか調べる
	std::vector<Model*>::iterator it = std::find(render_models.begin(), render_models.end(), model);
	if (it != render_models.end())
		return;

	render_models.push_back(model);
}

//	描画モデルの登録解除
void ModelRenderer::UnregisterRenderModel(Model* model)
{
	// モデルが既に登録されているか調べる
	std::vector<Model*>::iterator it = std::find(render_models.begin(), render_models.end(), model);
	if (it == render_models.end())
		return;

	render_models.erase(it);
}

//	描画モデル登録全解除
void ModelRenderer::ClearRenderModel()
{
	render_models.clear();
}
