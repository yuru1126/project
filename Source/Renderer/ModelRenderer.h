#pragma once

//#include "BaseRenderer.h"
#include "Graphics/Model/Model.h"

class ModelRenderer
{
public:
	ModelRenderer() {}
	~ModelRenderer() {}

	//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^
	void RegisterRenderModel(Model* model);

	//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^‰ğœ
	void UnregisterRenderModel(Model* model);

	//	•`‰æƒ‚ƒfƒ‹“o˜^‘S‰ğœ
	void ClearRenderModel();

	//  •`‰æƒ‚ƒfƒ‹”æ“¾
	int GetRenderModelCount() const { return static_cast<int>(render_models.size()); }
	
	//  •`‰æƒ‚ƒfƒ‹æ“¾
	Model* GetRenderModel(int index) { return render_models.at(index); }

	//  •`‰æƒ‚ƒfƒ‹”æ“¾
	int GetModelCount() const { return static_cast<int>(render_models.size()); }

	// •`‰æƒ‚ƒfƒ‹ƒŠƒXƒgæ“¾
	std::vector<Model*> GetRenderModels() { return render_models; }
private:
	std::vector<Model*>	render_models; // •`‰æƒ‚ƒfƒ‹ƒŠƒXƒg
};
