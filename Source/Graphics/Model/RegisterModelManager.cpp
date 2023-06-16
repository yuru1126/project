#include "Graphics/Model/RegisterModelManager.h"

//////    ���f���o�^,�����֘A    //////
void RegisterModelManager::RegisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : register_model)
    {
        mr->RegisterRenderModel(model);
    }
}
void RegisterModelManager::UnregisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : exclusion_model)
    {
        mr->UnregisterRenderModel(model);
    }
}

void RegisterModelManager::RegisterModel(Model* model)
{
    register_model.emplace_back(model);
}
void RegisterModelManager::ExclusionModel(Model* model)
{
    exclusion_model.emplace_back(model);
}


//////    �{���m�C�p    //////
void RegisterModelManager::VoronoiRegisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : voronoi_register_model)
    {
        mr->RegisterRenderModel(model);
    }
}
void RegisterModelManager::VoronoiUnregisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : voronoi_exclusion_model)
    {
        mr->UnregisterRenderModel(model);
    }
}

void RegisterModelManager::VoronoiRegisterModel(Model* model)
{
    voronoi_register_model.emplace_back(model);
}
void RegisterModelManager::VoronoiExclusionModel(Model* model)
{
    voronoi_exclusion_model.emplace_back(model);
}


///////    �|���S���p    //////
void RegisterModelManager::PolygonRegisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : polygon_register_model)
    {
        mr->RegisterRenderModel(model);
    }
}
void RegisterModelManager::PolygonUnregisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : polygon_exclusion_model)
    {
        mr->UnregisterRenderModel(model);
    }
}

void RegisterModelManager::PolygonRegisterModel(Model* model)
{
    polygon_register_model.emplace_back(model);
}
void RegisterModelManager::PolygonExclusionModel(Model* model)
{
    polygon_exclusion_model.emplace_back(model);
}


//////		�S�ē����ꍇ    //////
void RegisterModelManager::AllRegisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : exclusion_model)
    {
        mr->RegisterRenderModel(model);
    }
    for (Model* model : voronoi_exclusion_model)
    {
        mr->RegisterRenderModel(model);
    }
    for (Model* model : polygon_exclusion_model)
    {
        mr->RegisterRenderModel(model);
    }
}
void RegisterModelManager::AllUnregisterRenderModel(ModelRenderer* mr)
{
    for (Model* model : exclusion_model)
    {
        mr->UnregisterRenderModel(model);
    }
    for (Model* model : voronoi_exclusion_model)
    {
        mr->UnregisterRenderModel(model);
    }
    for (Model* model : polygon_exclusion_model)
    {
        mr->UnregisterRenderModel(model);
    }
}

//���f���폜
void RegisterModelManager::Clear()
{
    register_model.clear();
    exclusion_model.clear();
}
void RegisterModelManager::VoronoiClear()
{
    voronoi_register_model.clear();
    voronoi_exclusion_model.clear();
}
void RegisterModelManager::PolygonClear()
{
    polygon_register_model.clear();
    polygon_exclusion_model.clear();
}
void RegisterModelManager::AllClear()
{
    Clear();
    VoronoiClear();
    PolygonClear();
}