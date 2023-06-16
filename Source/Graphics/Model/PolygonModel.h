#pragma once
#include "Graphics/Model/Model.h"

class PolygonModel : public Model
{
public:
    PolygonModel(const char* filename) : Model(filename) {};
    ~PolygonModel() override {}

    void SetRenderContext(RenderContext* rc)override { rc->polygon_data = data; }

    void SetPolygonData(PolygonData da) { data = da; }
    void SetPolygonData(float time, float pos, float rot, float scale, float yplas, float dest = 0.0f)
    {
        data.time = time;

        data.position_factor = pos;
        data.rotation_factor = rot;
        data.scale_factor = scale;
        data.yplas_factor = yplas;
        data.destruction_factor = dest;
    }

private:
    PolygonData data{};
};