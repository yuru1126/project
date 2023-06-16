#pragma once
#include <vector>
#include <memory>

#include "Graphics/Model/Model.h"
#include "Renderer/ModelRenderer.h"

//���f���o�^�p�}�l�[�W���[
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

	//////    ���f���o�^,�����֘A    //////
	void RegisterRenderModel(ModelRenderer* mr);
	void UnregisterRenderModel(ModelRenderer* mr);

	void RegisterModel(Model* model);
	void ExclusionModel(Model* model);


	//////    �{���m�C�p    //////
	void VoronoiRegisterRenderModel(ModelRenderer* mr);
	void VoronoiUnregisterRenderModel(ModelRenderer* mr);
	
	void VoronoiRegisterModel(Model* model);
	void VoronoiExclusionModel(Model* model);


	///////    �|���S���p    //////
	void PolygonRegisterRenderModel(ModelRenderer* mr);
	void PolygonUnregisterRenderModel(ModelRenderer* mr);
	
	void PolygonRegisterModel(Model* model);
	void PolygonExclusionModel(Model* model);


	//////		�S�ē����ꍇ    //////
	void AllRegisterRenderModel(ModelRenderer* mr);
	void AllUnregisterRenderModel(ModelRenderer* mr);

	//���f���폜
	void Clear();
	void VoronoiClear();
	void PolygonClear();
	void AllClear();


private:
	//���f���o�^
	std::vector<Model*>     register_model;
	std::vector<Model*>     voronoi_register_model;//�{���m�C�p
	std::vector<Model*>     polygon_register_model;//�|���S���p


	//���f������
	std::vector<Model*>     exclusion_model;
	std::vector<Model*>     voronoi_exclusion_model;//�{���m�C�p
	std::vector<Model*>     polygon_exclusion_model;//�|���S���p

};