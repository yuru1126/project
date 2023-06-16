#include "Renderer/ModelRenderer.h"

//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^
void ModelRenderer::RegisterRenderModel(Model* model)
{

	// ƒ‚ƒfƒ‹‚ªŠù‚É“o˜^‚³‚ê‚Ä‚¢‚é‚©’²‚×‚é
	std::vector<Model*>::iterator it = std::find(render_models.begin(), render_models.end(), model);
	if (it != render_models.end())
		return;

	render_models.push_back(model);
}

//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^‰ğœ
void ModelRenderer::UnregisterRenderModel(Model* model)
{
	// ƒ‚ƒfƒ‹‚ªŠù‚É“o˜^‚³‚ê‚Ä‚¢‚é‚©’²‚×‚é
	std::vector<Model*>::iterator it = std::find(render_models.begin(), render_models.end(), model);
	if (it == render_models.end())
		return;

	render_models.erase(it);
}

//	•`‰æƒ‚ƒfƒ‹“o˜^‘S‰ğœ
void ModelRenderer::ClearRenderModel()
{
	render_models.clear();
}
