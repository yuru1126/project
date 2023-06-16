#include "Renderer/ModelRenderer.h"

//	�`�惂�f���̓o�^
void ModelRenderer::RegisterRenderModel(Model* model)
{

	// ���f�������ɓo�^����Ă��邩���ׂ�
	std::vector<Model*>::iterator it = std::find(render_models.begin(), render_models.end(), model);
	if (it != render_models.end())
		return;

	render_models.push_back(model);
}

//	�`�惂�f���̓o�^����
void ModelRenderer::UnregisterRenderModel(Model* model)
{
	// ���f�������ɓo�^����Ă��邩���ׂ�
	std::vector<Model*>::iterator it = std::find(render_models.begin(), render_models.end(), model);
	if (it == render_models.end())
		return;

	render_models.erase(it);
}

//	�`�惂�f���o�^�S����
void ModelRenderer::ClearRenderModel()
{
	render_models.clear();
}
