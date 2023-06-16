#pragma once
#include "Graphics/Model/Model.h"

class VoronoiModel : public Model
{
public:
    VoronoiModel(const char* filename) : Model(filename) {};
    ~VoronoiModel() override {}

    void SetRenderContext(RenderContext* rc)override { rc->voronoi_data = data; }

    void SetVoronoiData(VoronoiData da) { data = da; }
    void SetVoronoiData(DirectX::XMFLOAT4 lenge, DirectX::XMFLOAT4 time_line)
    {
        data.lenge = lenge;
        data.time_line = time_line;
    }

private:
    VoronoiData data{};
};