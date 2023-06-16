#pragma once

//#include "BaseRenderer.h"
#include "Graphics/Model/Model.h"

class ModelRenderer
{
public:
	ModelRenderer() {}
	~ModelRenderer() {}

	//	�`�惂�f���̓o�^
	void RegisterRenderModel(Model* model);

	//	�`�惂�f���̓o�^����
	void UnregisterRenderModel(Model* model);

	//	�`�惂�f���o�^�S����
	void ClearRenderModel();

	//  �`�惂�f�����擾
	int GetRenderModelCount() const { return static_cast<int>(render_models.size()); }
	
	//  �`�惂�f���擾
	Model* GetRenderModel(int index) { return render_models.at(index); }

	//  �`�惂�f�����擾
	int GetModelCount() const { return static_cast<int>(render_models.size()); }

	// �`�惂�f�����X�g�擾
	std::vector<Model*> GetRenderModels() { return render_models; }
private:
	std::vector<Model*>	render_models; // �`�惂�f�����X�g
};
