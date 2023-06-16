#include "Collison.h"

//球と球の交差判定
bool Collision::intersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB)
{
    //A→Bの単位ベクトルを算出
    using namespace DirectX;

    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR Lengthsq  = DirectX::XMVector3LengthSq(Vec);
   
    float lengthsq;
    DirectX::XMStoreFloat(&lengthsq, Lengthsq);

    //距離判定
    float range = radiusA + radiusB;
    if (range * range < lengthsq)
    {
        return false;
    }

    //AがBを押し出す
    Lengthsq = DirectX::XMPlaneNormalize(Vec);
    DirectX::XMVectorAdd(PositionA, DirectX::XMVectorScale(Lengthsq, range));
    DirectX::XMStoreFloat3(&outPositionB, PositionA);
    return true;
}

//円柱と円柱の交差判定
bool Collision::intersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB
)
{

    //Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    //Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
    


    //XZ平面での範囲チェック
    float ar = (positionA.x - positionB.x) * (positionA.x - positionB.x);
    float br = (positionA.z - positionB.z) * (positionA.z - positionB.z);
    float range = radiusA + radiusB;
    if (range * range < ar + br)
    {
        return false;
    }
    //AがBを押し出す
    float angl_set = -atan2f(positionA.x - positionB.x, positionA.z - positionB.z);
    //DirectX::XMFLOAT2 Bxz = { sinf(angl_set) * (radiusA + 0.5f) ,-cosf(angl_set) * (radiusA+0.5f) };
    //DirectX::XMFLOAT2 Bxz = { sinf(angl_set) * (radiusA) ,-cosf(angl_set) * (radiusA) };
    DirectX::XMFLOAT2 Bxz = { sinf(angl_set) * (range) ,-cosf(angl_set) * (range) };
    outPositionB.x = positionA.x + Bxz.x;
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + Bxz.y;
    return true;
}

//球と円柱の交差判定
bool Collision::intersectSphereVsCylinder(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius,
    float cylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition
)
{
    //Aの足元がBの頭より上なら当たっていない
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
    {
        return false;
    }
    //Aの頭がBの足元より下なら当たっていない
    if (spherePosition.y + sphereRadius < cylinderPosition.y)
    {
        return false;
    }

    float vx = cylinderPosition.x - spherePosition.x;
    float vz = cylinderPosition.z - spherePosition.z;
    float dist = sqrtf(vx * vx + vz * vz);
    float range = cylinderRadius + sphereRadius;
    if (range < dist)
    {
        return false;
    }
    vx /= dist;
    vz /= dist;
    outCylinderPosition.x = spherePosition.x + vx * range;
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = spherePosition.z + vz * range;
    return true;
}

//レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    //ワールド空間のレイの長さ
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //メッシュノード取得
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //レイをワールド空間からローカル空間へ変換
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart,InverseWorldTransform);
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd,InverseWorldTransform);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

        //レイの長さ
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        //三角形（面）との交差判定
        CONST std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                //三角形の頂点を抽出
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                //三角形の三辺ベクトルを算出
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                //三角形の法線ベクトルを算出
                DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

                //内積の結果が負でなければ裏向き
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
                // DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(WorldRayVec, Normal);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0)continue;

                //レイと平面の交点を算出
                using namespace DirectX;

                DirectX::XMVECTOR V = A - Start;
                DirectX::XMVECTOR T = DirectX::XMVector3Dot(V, Normal) / Dot;

                float t;
                DirectX::XMStoreFloat(&t, T);
                if (t < 0.0f || t > neart)continue;

                //DirectX::XMVECTOR Position = Start + Dir * T;
                DirectX::XMVECTOR Position = DirectX::XMVectorAdd(Start,
                    DirectX::XMVectorMultiply(Dir, T));

                //交点が三角形の内側にあるか判定
                //1つめ
                DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A,Position);
                //DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(AB,V1);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1,AB);
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Normal, Cross1);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0)continue;
                //2つめ

                //DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(Position, B);
                //DirectX::XMVECTOR Cross2 = DirectX::XMVectorSubtract(Position, C);
                //DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(V2, Cross2);
                //DirectX::XMStoreFloat(&dot, Dot2);
                //if (dot < 0)continue;
                
                DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B,Position);
                //DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(BC, V2);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2,BC);
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Normal, Cross2);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);
                if (dot2 < 0)continue;

                //3つめ
                //DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(Position, C);
                //DirectX::XMVECTOR Cross3 = DirectX::XMVectorSubtract(Position, A);
                //DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(V3, Cross3);
                //DirectX::XMStoreFloat(&dot, Dot3);
                //if (dot < 0)continue;
                
                DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
                //DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(CA, V3);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Normal, Cross3);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);
                if (dot3 < 0)continue;

                //最新距離を更新
                neart = t;

                //交点と法線を更新
                HitPosition = Position;
                HitNormal = Normal;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            //ローカル空間からワールド空間へ変換
            DirectX::XMVECTOR WoldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                                                                                WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WoldPosition,WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            //ヒット情報保存
            if (result.distance > distance) 
            {
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal,
                    WorldTransform);
                result.distance = distance;
                result.materialIndex = materialIndex;
                result.position = WoldPosition;
                result.normal = DirectX::XMVector3Normalize(WorldNormal);
                hit = true;
            }
        }
    }
    return hit;
}

bool Collision::IntersectRayVsModelEST(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    //ワールド空間のレイの長さ
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //メッシュノード取得
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //レイをワールド空間からローカル空間へ変換
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

        //AABB判定用
        DirectX::XMFLOAT3 st{};
        DirectX::XMFLOAT3 en{};
        DirectX::XMStoreFloat3(&st, Start);
        DirectX::XMStoreFloat3(&en, End);
        AABB f;
        f.center.x = (st.x + en.x) / 2.0f;
        f.center.y = (st.y + en.y) / 2.0f;
        f.center.z = (st.z + en.z) / 2.0f;
        f.radius.x = (std::max)(f.center.x - st.x, st.x - f.center.x);
        f.radius.y = (std::max)(f.center.y - st.y, st.y - f.center.y);
        f.radius.z = (std::max)(f.center.z - st.z, st.z - f.center.z);


        //レイの長さ
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        //三角形（面）との交差判定
        CONST std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                //三角形の頂点を抽出
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMFLOAT3 p[3] = { a.position ,b.position ,c.position };
                DirectX::XMFLOAT3 min = p[0];
                DirectX::XMFLOAT3 max = p[0];
                for (int i = 1; i < 3; i++)
                {
                    min.x = (std::min)(min.x, p[i].x);
                    min.y = (std::min)(min.y, p[i].y);
                    min.z = (std::min)(min.z, p[i].z);

                    max.x = (std::max)(max.x, p[i].x);
                    max.y = (std::max)(max.y, p[i].y);
                    max.z = (std::max)(max.z, p[i].z);
                }
                AABB s;
                s.center.x = (min.x + max.x) / 2.0f;
                s.center.y = (min.y + max.y) / 2.0f;
                s.center.z = (min.z + max.z) / 2.0f;
                s.radius.x = (std::max)(s.center.x - min.x, min.x - s.center.x);
                s.radius.y = (std::max)(s.center.y - min.y, min.y - s.center.y);
                s.radius.z = (std::max)(s.center.z - min.z, min.z - s.center.z);
                if (!AABBVsAABB(f, s))continue;

                DirectX::XMVECTOR POS[3] = {
                    DirectX::XMLoadFloat3(&a.position),
                    DirectX::XMLoadFloat3(&b.position),
                    DirectX::XMLoadFloat3(&c.position) };

                DirectX::XMVECTOR outPos;
                DirectX::XMVECTOR outNormal;

                if (CollideRayVsTriangle(Start, Dir, &neart, POS, &outPos,&outNormal))
                {

                    //交点と法線を更新
                    HitPosition = outPos;
                    HitNormal = outNormal;
                    materialIndex = subset.materialIndex;
                }
            }
            if (materialIndex >= 0)
            {

                //ローカル空間からワールド空間へ変換
                DirectX::XMVECTOR WoldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                    WorldTransform);
                DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WoldPosition, WorldStart);
                DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
                float distance;
                DirectX::XMStoreFloat(&distance, WorldCrossLength);

                //ヒット情報保存
                if (result.distance > distance)
                {

                    DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal,
                        WorldTransform);
                    result.distance = distance;
                    result.materialIndex = materialIndex;
                    result.position = WoldPosition;
                    result.normal = DirectX::XMVector3Normalize(WorldNormal);
                    hit = true;
                }
            }
        }
    }

    return hit;
}


#if 0
    //スフィアキャストとモデルの交差判定  
    bool Collision::IntersectSphereCastVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const float radius,
        const Model* model,
        HitResult& result
    )
    {
        DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
        DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
        DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
        DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

        //ワールド空間のレイの長さ
        DirectX::XMStoreFloat(&result.distance, WorldRayLength);

        //取得したレイの長さを保持
        float keep_lenge = FLT_MAX;

        bool hit = false;
        const ModelResource* resource = model->GetResource();
        for (const ModelResource::Mesh& mesh : resource->GetMeshes())
        {
            //メッシュノード取得
            const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

            //レイをワールド空間からローカル空間へ変換
            DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
            DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

            DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
            DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
            DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
            DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

            //レイの長さ
            float neart;
            DirectX::XMStoreFloat(&neart, Length);

            //三角形（面）との交差判定
            CONST std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
            const std::vector<UINT> indices = mesh.indices;

            int materialIndex = -1;
            DirectX::XMVECTOR HitPosition;
            DirectX::XMVECTOR HitNormal;
            for (const ModelResource::Subset& subset : mesh.subsets)
            {
                for (UINT i = 0; i < subset.indexCount; i += 3)
                {
                    UINT index = subset.startIndex + i;

                    //三角形の頂点を抽出
                    const ModelResource::Vertex& a = vertices.at(indices.at(index));
                    const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                    const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                    DirectX::XMVECTOR POS[3] = {
                        DirectX::XMLoadFloat3(&a.position),
                        DirectX::XMLoadFloat3(&b.position),
                        DirectX::XMLoadFloat3(&c.position) };

                    DirectX::XMVECTOR outPos;
                    DirectX::XMVECTOR outNormal;

                    if (IntersectSphereCastVsTriangle(Start, End, radius, POS, result))
                    {
                        if (keep_lenge > result.distance)
                        {

                            keep_lenge = result.distance;
                            //交点と法線を更新
                            HitPosition = result.position;
                            HitNormal = result.normal;
                            materialIndex = subset.materialIndex;
                            //hit = true;
                        }
                        else
                        {
                            result.distance = keep_lenge;
                        }
                    }
                    else
                    {
                        result.distance = keep_lenge;
                    }
                }
                //if (materialIndex >= 0 && false)
                if (materialIndex >= 0)
                {
                    //ローカル空間からワールド空間へ変換
                    DirectX::XMVECTOR WoldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                        WorldTransform);
                    DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WoldPosition, WorldStart);
                    DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
                    float distance;
                    DirectX::XMStoreFloat(&distance, WorldCrossLength);

                    //ヒット情報保存
                    if (result.distance > distance)
                    {

                        DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal,
                            WorldTransform);
                        result.distance = distance;
                        result.materialIndex = materialIndex;
                        result.position = WoldPosition;
                        result.normal = DirectX::XMVector3Normalize(WorldNormal);
                        hit = true;
                    }
                }
            }
        }
        result.distance = keep_lenge;
        return hit;
    }
#else
//スフィアキャストとモデルの交差判定  
bool Collision::IntersectSphereCastVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const float radius,
    const Model* model,
    HitResult& result
)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    AABB f;
    f.center.x = (start.x + end.x) / 2.0f;
    f.center.y = (start.y + end.y) / 2.0f;
    f.center.z = (start.z + end.z) / 2.0f;
    f.radius.x = (std::max)(f.center.x - start.x, start.x - f.center.x);
    f.radius.y = (std::max)(f.center.y - start.y, start.y - f.center.y);
    f.radius.z = (std::max)(f.center.z - start.z, start.z - f.center.z);


    //ワールド空間のレイの長さ
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    //取得したレイの長さを保持
    float keep_lenge = FLT_MAX;

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //メッシュノード取得
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //三角形の頂点をローカル空間からワールド空間へ変換
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);

        //三角形（面）との交差判定
        CONST std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                //三角形の頂点を抽出
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR POS[3] = {
                    DirectX::XMLoadFloat3(&a.position),
                    DirectX::XMLoadFloat3(&b.position),
                    DirectX::XMLoadFloat3(&c.position) };

                //三角形の頂点をローカル空間からワールド空間へ変換
                POS[0] = DirectX::XMVector3TransformCoord(POS[0], WorldTransform);
                POS[1] = DirectX::XMVector3TransformCoord(POS[1], WorldTransform);
                POS[2] = DirectX::XMVector3TransformCoord(POS[2], WorldTransform);

                DirectX::XMFLOAT3 min = { FLT_MAX ,FLT_MAX ,FLT_MAX };
                DirectX::XMFLOAT3 max = { FLT_MIN ,FLT_MIN ,FLT_MIN };
                for (int i = 0; i < 3; i++)
                {
                    DirectX::XMFLOAT3 p{};
                    DirectX::XMStoreFloat3(&p,POS[i]);

                    min.x = (std::min)(min.x, p.x);
                    min.y = (std::min)(min.y, p.y);
                    min.z = (std::min)(min.z, p.z);

                    max.x = (std::max)(max.x, p.x);
                    max.y = (std::max)(max.y, p.y);
                    max.z = (std::max)(max.z, p.z);
                }
                AABB s;
                s.center.x = (min.x + max.x) / 2.0f;
                s.center.y = (min.y + max.y) / 2.0f;
                s.center.z = (min.z + max.z) / 2.0f;
                s.radius.x = (std::max)(s.center.x - min.x, min.x - s.center.x);
                s.radius.y = (std::max)(s.center.y - min.y, min.y - s.center.y);
                s.radius.z = (std::max)(s.center.z - min.z, min.z - s.center.z);
                if (!AABBVsAABB(f, s, radius))continue;


                HitResult hitr;
                if (IntersectSphereCastVsTriangle(WorldStart, WorldEnd, radius, POS, hitr))
                //if (IntersectSphereCastVsTriangleEST(WorldStart, WorldEnd, radius, POS, hitr))
                {
                    if (keep_lenge > hitr.distance && result.distance > hitr.distance)
                    //if (keep_lenge > hitr.distance)
                    {
                        keep_lenge = hitr.distance;
                        result.distance = hitr.distance;
                        //交点と法線を更新
                        result.position = hitr.position;
                        result.normal = hitr.normal;
                        result.materialIndex = subset.materialIndex;
                        hit = true;
                    }
                }
            }
        }
    }
    return hit;
}
#endif


// 外部の点に対する三角形内部の最近点を算出する
bool Collision::ClosestPosPointTriangle(
    const DirectX::XMVECTOR& Point,
    const DirectX::XMVECTOR TrianglePos[3],
    DirectX::XMVECTOR* nearPos)
{
    DirectX::XMVECTOR dummyNearTriPos1 = {};
    DirectX::XMVECTOR dummyNearTriPos2 = {};

    return ClosestPosPointTriangle(Point, TrianglePos, nearPos, &dummyNearTriPos1, &dummyNearTriPos2);
}

bool Collision::ClosestPosPointTriangle(
    const DirectX::XMVECTOR& Point,
    const DirectX::XMVECTOR TrianglePos[3],
    DirectX::XMVECTOR* nearPos,
    DirectX::XMVECTOR* nearTriPos1,
    DirectX::XMVECTOR* nearTriPos2)
{
    // pointがTrianglePos[0]の外側の頂点領域にあるかチェック
    DirectX::XMVECTOR Vec01 = DirectX::XMVectorSubtract(TrianglePos[1], TrianglePos[0]);
    DirectX::XMVECTOR Vec02 = DirectX::XMVectorSubtract(TrianglePos[2], TrianglePos[0]);
    DirectX::XMVECTOR Vec0P = DirectX::XMVectorSubtract(Point, TrianglePos[0]);
    float d1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec0P));
    float d2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec0P));

    if (d1 <= 0.0f && d2 <= 0.0f)
    {
        *nearPos = TrianglePos[0];
        *nearTriPos1 = TrianglePos[0];
        return false;
    }

    // pointがTrianglePos[1]の外側の頂点領域にあるかチェック
    DirectX::XMVECTOR Vec1P = DirectX::XMVectorSubtract(Point, TrianglePos[1]);
    float d3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec1P));
    float d4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec1P));

    if (d3 >= 0.0f && d4 <= d3)
    {
        *nearPos = TrianglePos[1];
        *nearTriPos1 = TrianglePos[1];
        return false;
    }

    // pointがTrianglePos[0]とTrianglePos[1]の外側の辺領域にあるかチェック
    float scalar3_01 = d1 * d4 - d3 * d2;

    if (scalar3_01 <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
    {
        float t = d1 / (d1 - d3);
        *nearPos = DirectX::XMVectorAdd(TrianglePos[0], DirectX::XMVectorScale(Vec01, t));
        *nearTriPos1 = TrianglePos[0];
        *nearTriPos2 = TrianglePos[1];
        return false;
    }

    // pointがTrianglePos[2]の外側の頂点領域にあるかチェック
    DirectX::XMVECTOR Vec2P = DirectX::XMVectorSubtract(Point, TrianglePos[2]);
    float d5 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec2P));
    float d6 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec2P));

    if (d6 >= 0.0f && d5 <= d6)
    {
        *nearPos = TrianglePos[2];
        *nearTriPos1 = TrianglePos[2];
        return false;
    }

    // pointがTrianglePos[0]とTrianglePos[2]の外側の辺領域にあるかチェック
    float scalar3_02 = d5 * d2 - d1 * d6;

    if (scalar3_02 <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
    {
        float t = d2 / (d2 - d6);
        *nearPos = DirectX::XMVectorAdd(TrianglePos[0], DirectX::XMVectorScale(Vec02, t));
        *nearTriPos1 = TrianglePos[0];
        *nearTriPos2 = TrianglePos[2];
        return false;
    }

    // pointがTrianglePos[1]とTrianglePos[2]の外側の辺領域にあるかチェック
    float scalar3_12 = d3 * d6 - d5 * d4;

    if (scalar3_12 <= 0.0f && d4 >= d3 && d5 >= d6)
    {
        float t = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        *nearPos = DirectX::XMVectorAdd(TrianglePos[1], DirectX::XMVectorScale(DirectX::XMVectorSubtract(TrianglePos[2], TrianglePos[1]), t));
        *nearTriPos1 = TrianglePos[1];
        *nearTriPos2 = TrianglePos[2];
        return false;
    }

    // ここまでくれば、nearPosは三角形の内部にある
    float denom = 1.0f / (scalar3_01 + scalar3_02 + scalar3_12);
    float t01 = scalar3_02 * denom;
    float t02 = scalar3_01 * denom;
    *nearPos = DirectX::XMVectorAdd(TrianglePos[0], DirectX::XMVectorAdd(DirectX::XMVectorScale(Vec01, t01), DirectX::XMVectorScale(Vec02, t02)));
    return true;
}

// 球と三角形の交差判定
bool Collision::IntersectSphereVsTriangle(
    const DirectX::XMVECTOR& spherePos,
    const float radius,
    const DirectX::XMVECTOR TrianglePos[3],
    DirectX::XMVECTOR* nearPos)
{
    ClosestPosPointTriangle(spherePos, TrianglePos, nearPos);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(*nearPos, spherePos);
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec)) <= (radius * radius);
}

// レイと球の交差判定
bool Collision::IntersectRayVsSphere(
    const DirectX::XMVECTOR& start,
    const DirectX::XMVECTOR& end,
    const DirectX::XMVECTOR& spherePos,
    const float radius,
    DirectX::XMVECTOR* hitPos,
    float* hitDistance)
{
    DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(end, start);
    DirectX::XMVECTOR directionNormalize = DirectX::XMVector3Normalize(direction);
    DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(spherePos, start);
    float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, directionNormalize));
    float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;

    if (distSq < radius * radius)
    {
        *hitDistance = projection - sqrtf(radius * radius - distSq);
        if (*hitDistance > 0.0f)
        {
            if (*hitDistance < DirectX::XMVectorGetX(DirectX::XMVector3Length(direction)))
            {
                *hitPos = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(directionNormalize, *hitDistance));
                return true;
            }
        }
    }

    *hitDistance = 0.0f;
    *hitPos = {};

    return false;
}

// レイと円柱の交差判定
bool Collision::IntersectRayVsCylinder(
    const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end,
    const DirectX::XMVECTOR& startCylinder, const DirectX::XMVECTOR& endCylinder,
    float radius,
    DirectX::XMVECTOR* hitPos,
    float* hitDistance)
{
    // 参考資料：ゲームプログラミングのためのリアルタイム衝突判定 P.194～198を改良
    DirectX::XMVECTOR d = DirectX::XMVectorSubtract(endCylinder, startCylinder);
    DirectX::XMVECTOR m = DirectX::XMVectorSubtract(start, startCylinder);
    DirectX::XMVECTOR n = DirectX::XMVectorSubtract(end, start);

    float md = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, d));
    float nd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
    float dd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, d));

    // 線分が円柱のどちらかの底面に対して完全に外側にあるかどうかを判定
    if (md < 0.0f && md + nd < 0.0f)
    {
        return false;	// 線分が円柱のstartCylinderの外側にある
    }
    if (md > dd && md + nd > dd)
    {
        return false;	// 線分が円柱のendCylinderの外側にある
    }

    float nn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, n));
    float a = dd * nn - nd * nd;
    float k = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, m)) - radius * radius;
    float c = dd * k - md * md;

    // 線分が円柱の軸に対して平行
    if (fabsf(a) < 0.0001f)		// 誤差が出やすい計算なので閾値は大きめ（0.0001f）
    {
        if (c > 0.0f) return false;	// 線分は円柱の外側
        return true;
    }

    // 線分が円柱の軸に対して平行でない
    // 円柱の表面を表す陰関数方程式と直線の方程式の解を求めて交差判定を行う。
    float mn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, n));
    float b = dd * mn - nd * md;
    float D = b * b - a * c;	// 判別式

    if (D < 0) return false;	// 実数解がないので交差していない

    *hitDistance = (-b - sqrtf(D)) / a;
    if (*hitDistance < 0.0f)
    {
        *hitDistance = (-b + sqrtf(D)) / a;
        if (*hitDistance < 0.0f)
        {
            return false;	// 交点が線分の外側にあり交差していない
        }
    }
    else if (*hitDistance > 1.0f)
    {
        *hitDistance = (-b + sqrtf(D)) / a;
        if (*hitDistance > 1.0f)
        {
            return false;	// 交点が線分の外側にあり交差していない
        }
    }

    if (md + *hitDistance * nd < 0.0f)
    {
        // 円柱のstartCylinder側の底面の外で交差
        *hitDistance = -md / nd;

        // Dot(S(t) - startCylinder, S(t) - startCylinder) <= cylinder.rad * cylinder.rad を満たす場合、交差している
        DirectX::XMVECTOR St = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(n, *hitDistance));
        St = DirectX::XMVectorSubtract(St, startCylinder);
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(St, St)) > radius * radius)
        {
            return false;
        }
    }
    else if (md + *hitDistance * nd > dd)
    {
        // 円柱のendCylinder側の底面の外で交差
        *hitDistance = (dd - md) / nd;

        // Dot(S(t) - endCylinder, S(t) - endCylinder) <= cylinder.rad * cylinder.rad を満たす場合、交差している
        DirectX::XMVECTOR St = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(n, *hitDistance));
        St = DirectX::XMVectorSubtract(St, endCylinder);
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(St, St)) > radius * radius)
        {
            return false;
        }
    }

    // 線分が円柱の底面と底面の間で交差している
    *hitPos = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(n, *hitDistance));
    *hitDistance *= DirectX::XMVectorGetX(DirectX::XMVector3Length(n));
    return true;
}


bool Collision::CollideRayVsTriangle(
     DirectX::XMVECTOR& rayStart,  DirectX::XMVECTOR& rayDirection,  float* rayDist,  DirectX::XMVECTOR TrianglePos[3],
    DirectX::XMVECTOR* outPos, DirectX::XMVECTOR* outNormal)
{
    DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(TrianglePos[1], TrianglePos[0]);
    DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(TrianglePos[2], TrianglePos[0]);
    DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
    DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStart, DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, *rayDist)));
    float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

    if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
    {
        if (fabs(d) > 1e-6f)	// 平行確認
        {
            DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStart, TrianglePos[0]);
            float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
            if (t >= 0.0f && t < d)
            {
                DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
                float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
                if (v >= 0.0f && v <= d)
                {
                    float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
                    if (w > 0.0f && v + w <= d)
                    {
                        *outPos = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, *rayDist * t / d));
                        *rayDist = *rayDist * t / d;
                        *outNormal = norm;
                        return true;
                    }
                }
            }
        }
    }

    *outPos = {};
    *outNormal = {};
    return false;
}

// スフィアキャスト 簡易版
bool Collision::IntersectSphereCastVsTriangleEST(
    const DirectX::XMVECTOR& start,
    const DirectX::XMVECTOR& end,
    const float radius,
    const DirectX::XMVECTOR trianglePos[3],
    HitResult& result)
{
    // 三角形とスタート位置における球が交差している場合は、falseで終了する
    if (IntersectSphereVsTriangle(start, radius, trianglePos, &result.position)) return false;

    DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(end, start);

    DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
    DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
    DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
    DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(start, end);
    float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

    if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
    {
        if (fabs(d) > 1e-6f)	// 平行確認
        {
            // レイの始点を三角形の法線を元に球表面の最短点に移動させる
            DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVectorNegate(norm)), radius);
            DirectX::XMVECTOR startFix = DirectX::XMVectorAdd(start, fixVec);

            DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(startFix, trianglePos[0]);
            float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
            if (t < d)	// レイの向きと長さ確認
            {
                // レイと三角形を含む平面の交点の算出
                DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
                float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
                float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
                t /= d;
                result.position = DirectX::XMVectorAdd(startFix, DirectX::XMVectorScale(direction, t));

                if (v >= 0.0f && v <= d && w > 0.0f && v + w <= d)
                {
                    // レイと三角形を含む平面の交点が三角形の内部
                    result.distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(direction)) * t;
                    result.normal = norm;
                    result.triangleVerts[0] = trianglePos[0];
                    result.triangleVerts[1] = trianglePos[1];
                    result.triangleVerts[2] = trianglePos[2];
                    return true;
                }
                else
                {
                    // レイと三角形を含む平面の交点が三角形の外部

                    // 交点に一番近い三角形内部の点を求める
                    DirectX::XMVECTOR nearPos = {};
                    ClosestPosPointTriangle(result.position, trianglePos, &nearPos);

                    // 交点に一番近い三角形内部の点から、逆向きのレイを発射し、レイの始点にある球体と交差するか確認
                    // 交差した場合、交点に一番近い三角形内部の点がスフィアキャストの三角形との交点になる
                    if (IntersectRayVsSphere(nearPos, DirectX::XMVectorAdd(nearPos, DirectX::XMVectorNegate(direction)), start, radius, &result.position, &result.distance))
                    {
                        result.position = nearPos;
                        result.normal = norm;
                        result.triangleVerts[0] = trianglePos[0];
                        result.triangleVerts[1] = trianglePos[1];
                        result.triangleVerts[2] = trianglePos[2];
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

// スフィアキャスト 完全版
bool Collision::IntersectSphereCastVsTriangle(
    const DirectX::XMVECTOR& start,
    const DirectX::XMVECTOR& end,
    const float radius,
    const DirectX::XMVECTOR trianglePos[3],
    HitResult& result)
{
    // 三角形とスタート位置における球が交差している場合は、falseで終了する
    if (IntersectSphereVsTriangle(start, radius, trianglePos, &result.position)) return false;

    DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
    DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
    DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
    DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(start, end);
    float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));
    result.distance = FLT_MAX;

    if (d >= 0.0f)	// 表側から交差しているときのみ判定を行う
    {
        // 三角形の各頂点を法線を元に球半径だけ移動させる
        DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(norm), radius);

        // 移動後の三角形と交差するなら、元の三角形の内部でスフィアキャストが交差することが確定
        DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(start, DirectX::XMVectorAdd(trianglePos[0], fixVec));
        float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
        if (t >= 0.0f && t < d)		// レイの向きと長さ確認
        {
            DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
            float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
            if (v >= 0.0f && v <= d)
            {
                float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
                if (w > 0.0f && v + w <= d)
                {
                    DirectX::XMVECTOR crossPos = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(qp, -t / d));
                    result.distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(crossPos, start)));
                    result.position = DirectX::XMVectorSubtract(crossPos, fixVec);
                    result.normal = norm;
                    result.triangleVerts[0] = trianglePos[0];
                    result.triangleVerts[1] = trianglePos[1];
                    result.triangleVerts[2] = trianglePos[2];

                    return true;
                }
            }
        }

        // 面領域で交差がなければ、ボロノイの各頂点領域、辺領域で交差判定を行い、最短距離を算出する
        DirectX::XMVECTOR position = {};
        float distance = 0.0f;
        DirectX::XMVECTOR directionNormalize = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(end, start));

        // trianglePos[0] 頂点領域のチェック
        DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(trianglePos[0], start);
        float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, directionNormalize));
        float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;
        if (projection > 0.0f)
        {
            if (distSq < radius * radius)
            {
                distance = projection - sqrtf(radius * radius - distSq);
                if (result.distance > distance)
                {
                    result.position = trianglePos[0];
                    result.distance = distance;
                    result.normal = norm;
                    result.triangleVerts[0] = trianglePos[0];
                    result.triangleVerts[1] = trianglePos[1];
                    result.triangleVerts[2] = trianglePos[2];
                }
            }
        }

        // trianglePos[1] 頂点領域のチェック
        ray2sphere = DirectX::XMVectorSubtract(trianglePos[1], start);
        projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, directionNormalize));
        distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;
        if (projection > 0.0f)
        {
            if (distSq < radius * radius)
            {
                distance = projection - sqrtf(radius * radius - distSq);
                if (result.distance > distance)
                {
                    result.position = trianglePos[1];
                    result.distance = distance;
                    result.normal = norm;
                    result.triangleVerts[0] = trianglePos[0];
                    result.triangleVerts[1] = trianglePos[1];
                    result.triangleVerts[2] = trianglePos[2];
                }
            }
        }

        // trianglePos[2] 頂点領域のチェック
        ray2sphere = DirectX::XMVectorSubtract(trianglePos[2], start);
        projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, directionNormalize));
        if (projection > 0.0f)
        {
            distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;
            if (distSq < radius * radius)
            {
                distance = projection - sqrtf(radius * radius - distSq);
                if (result.distance > distance)
                {
                    result.position = trianglePos[2];
                    result.distance = distance;
                    result.normal = norm;
                    result.triangleVerts[0] = trianglePos[0];
                    result.triangleVerts[1] = trianglePos[1];
                    result.triangleVerts[2] = trianglePos[2];
                }
            }
        }

        // trianglePos[0]-trianglePos[1] 辺領域のチェック
        if (IntersectRayVsCylinder(start, end, trianglePos[0], trianglePos[1], radius, &position, &distance))
        {
            if (result.distance > distance)
            {
                DirectX::XMVECTOR vec0p = DirectX::XMVectorSubtract(position, trianglePos[0]);
                DirectX::XMVECTOR vec01Norm = DirectX::XMVector3Normalize(ab);
                result.position = DirectX::XMVectorMultiplyAdd(DirectX::XMVector3Dot(vec0p, vec01Norm), vec01Norm, trianglePos[0]);
                result.distance = distance;
                result.normal = norm;
                result.triangleVerts[0] = trianglePos[0];
                result.triangleVerts[1] = trianglePos[1];
                result.triangleVerts[2] = trianglePos[2];
            }
        }

        // trianglePos[0]-trianglePos[2] 辺領域のチェック
        if (IntersectRayVsCylinder(start, end, trianglePos[0], trianglePos[2], radius, &position, &distance))
        {
            if (result.distance > distance)
            {
                DirectX::XMVECTOR vec0p = DirectX::XMVectorSubtract(position, trianglePos[0]);
                DirectX::XMVECTOR vec02Norm = DirectX::XMVector3Normalize(ac);
                result.position = DirectX::XMVectorMultiplyAdd(DirectX::XMVector3Dot(vec0p, vec02Norm), vec02Norm, trianglePos[0]);
                result.distance = distance;
                result.normal = norm;
                result.triangleVerts[0] = trianglePos[0];
                result.triangleVerts[1] = trianglePos[1];
                result.triangleVerts[2] = trianglePos[2];
            }
        }

        // trianglePos[1]-trianglePos[2] 辺領域のチェック
        if (IntersectRayVsCylinder(start, end, trianglePos[1], trianglePos[2], radius, &position, &distance))
        {
            if (result.distance > distance)
            {
                DirectX::XMVECTOR vec1p = DirectX::XMVectorSubtract(position, trianglePos[1]);
                DirectX::XMVECTOR vec12Norm = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(trianglePos[2], trianglePos[1]));
                result.position = DirectX::XMVectorMultiplyAdd(DirectX::XMVector3Dot(vec1p, vec12Norm), vec12Norm, trianglePos[1]);
                result.distance = distance;
                result.normal = norm;
                result.triangleVerts[0] = trianglePos[0];
                result.triangleVerts[1] = trianglePos[1];
                result.triangleVerts[2] = trianglePos[2];
            }
        }
    }

    return result.distance != FLT_MAX;
}


//簡易的なAABBの当たり判定
bool Collision::AABBVsAABB(AABB first, AABB second, float lenge)
{
    //Xの交差判定
    if (first.center.x - first.radius.x > second.center.x + second.radius.x + lenge)
    {
        return false;
    }
    if (first.center.x + first.radius.x < second.center.x - second.radius.x - lenge)
    {
        return false;
    }
    //Yの交差判定
    if (first.center.y - first.radius.y > second.center.y + second.radius.y + lenge)
    {
        return false;
    }
    if (first.center.y + first.radius.y < second.center.y - second.radius.y - lenge)
    {
        return false;
    }
    //Zの交差判定
    if (first.center.z - first.radius.z > second.center.z + second.radius.z + lenge)
    {
        return false;
    }
    if (first.center.z + first.radius.z < second.center.z - second.radius.z - lenge)
    {
        return false;
    }
    return true;
}