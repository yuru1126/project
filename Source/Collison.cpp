#include "Collison.h"

//���Ƌ��̌�������
bool Collision::intersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB)
{
    //A��B�̒P�ʃx�N�g�����Z�o
    using namespace DirectX;

    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR Lengthsq  = DirectX::XMVector3LengthSq(Vec);
   
    float lengthsq;
    DirectX::XMStoreFloat(&lengthsq, Lengthsq);

    //��������
    float range = radiusA + radiusB;
    if (range * range < lengthsq)
    {
        return false;
    }

    //A��B�������o��
    Lengthsq = DirectX::XMPlaneNormalize(Vec);
    DirectX::XMVectorAdd(PositionA, DirectX::XMVectorScale(Lengthsq, range));
    DirectX::XMStoreFloat3(&outPositionB, PositionA);
    return true;
}

//�~���Ɖ~���̌�������
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

    //A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    //A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
    


    //XZ���ʂł͈̔̓`�F�b�N
    float ar = (positionA.x - positionB.x) * (positionA.x - positionB.x);
    float br = (positionA.z - positionB.z) * (positionA.z - positionB.z);
    float range = radiusA + radiusB;
    if (range * range < ar + br)
    {
        return false;
    }
    //A��B�������o��
    float angl_set = -atan2f(positionA.x - positionB.x, positionA.z - positionB.z);
    //DirectX::XMFLOAT2 Bxz = { sinf(angl_set) * (radiusA + 0.5f) ,-cosf(angl_set) * (radiusA+0.5f) };
    //DirectX::XMFLOAT2 Bxz = { sinf(angl_set) * (radiusA) ,-cosf(angl_set) * (radiusA) };
    DirectX::XMFLOAT2 Bxz = { sinf(angl_set) * (range) ,-cosf(angl_set) * (range) };
    outPositionB.x = positionA.x + Bxz.x;
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + Bxz.y;
    return true;
}

//���Ɖ~���̌�������
bool Collision::intersectSphereVsCylinder(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius,
    float cylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition
)
{
    //A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
    {
        return false;
    }
    //A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
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

//���C�ƃ��f���̌�������
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

    //���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart,InverseWorldTransform);
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd,InverseWorldTransform);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

        //���C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        //�O�p�`�i�ʁj�Ƃ̌�������
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

                //�O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                //�O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                //�O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

                //���ς̌��ʂ����łȂ���Η�����
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
                // DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(WorldRayVec, Normal);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0)continue;

                //���C�ƕ��ʂ̌�_���Z�o
                using namespace DirectX;

                DirectX::XMVECTOR V = A - Start;
                DirectX::XMVECTOR T = DirectX::XMVector3Dot(V, Normal) / Dot;

                float t;
                DirectX::XMStoreFloat(&t, T);
                if (t < 0.0f || t > neart)continue;

                //DirectX::XMVECTOR Position = Start + Dir * T;
                DirectX::XMVECTOR Position = DirectX::XMVectorAdd(Start,
                    DirectX::XMVectorMultiply(Dir, T));

                //��_���O�p�`�̓����ɂ��邩����
                //1��
                DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A,Position);
                //DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(AB,V1);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1,AB);
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Normal, Cross1);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0)continue;
                //2��

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

                //3��
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

                //�ŐV�������X�V
                neart = t;

                //��_�Ɩ@�����X�V
                HitPosition = Position;
                HitNormal = Normal;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            //���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WoldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                                                                                WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WoldPosition,WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            //�q�b�g���ۑ�
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

    //���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

        //AABB����p
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


        //���C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        //�O�p�`�i�ʁj�Ƃ̌�������
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

                //�O�p�`�̒��_�𒊏o
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

                    //��_�Ɩ@�����X�V
                    HitPosition = outPos;
                    HitNormal = outNormal;
                    materialIndex = subset.materialIndex;
                }
            }
            if (materialIndex >= 0)
            {

                //���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
                DirectX::XMVECTOR WoldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                    WorldTransform);
                DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WoldPosition, WorldStart);
                DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
                float distance;
                DirectX::XMStoreFloat(&distance, WorldCrossLength);

                //�q�b�g���ۑ�
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
    //�X�t�B�A�L���X�g�ƃ��f���̌�������  
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

        //���[���h��Ԃ̃��C�̒���
        DirectX::XMStoreFloat(&result.distance, WorldRayLength);

        //�擾�������C�̒�����ێ�
        float keep_lenge = FLT_MAX;

        bool hit = false;
        const ModelResource* resource = model->GetResource();
        for (const ModelResource::Mesh& mesh : resource->GetMeshes())
        {
            //���b�V���m�[�h�擾
            const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

            //���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
            DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
            DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

            DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
            DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
            DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
            DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

            //���C�̒���
            float neart;
            DirectX::XMStoreFloat(&neart, Length);

            //�O�p�`�i�ʁj�Ƃ̌�������
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

                    //�O�p�`�̒��_�𒊏o
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
                            //��_�Ɩ@�����X�V
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
                    //���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
                    DirectX::XMVECTOR WoldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                        WorldTransform);
                    DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WoldPosition, WorldStart);
                    DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
                    float distance;
                    DirectX::XMStoreFloat(&distance, WorldCrossLength);

                    //�q�b�g���ۑ�
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
//�X�t�B�A�L���X�g�ƃ��f���̌�������  
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


    //���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    //�擾�������C�̒�����ێ�
    float keep_lenge = FLT_MAX;

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //�O�p�`�̒��_�����[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);

        //�O�p�`�i�ʁj�Ƃ̌�������
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

                //�O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR POS[3] = {
                    DirectX::XMLoadFloat3(&a.position),
                    DirectX::XMLoadFloat3(&b.position),
                    DirectX::XMLoadFloat3(&c.position) };

                //�O�p�`�̒��_�����[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
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
                        //��_�Ɩ@�����X�V
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


// �O���̓_�ɑ΂���O�p�`�����̍ŋߓ_���Z�o����
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
    // point��TrianglePos[0]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
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

    // point��TrianglePos[1]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
    DirectX::XMVECTOR Vec1P = DirectX::XMVectorSubtract(Point, TrianglePos[1]);
    float d3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec1P));
    float d4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec1P));

    if (d3 >= 0.0f && d4 <= d3)
    {
        *nearPos = TrianglePos[1];
        *nearTriPos1 = TrianglePos[1];
        return false;
    }

    // point��TrianglePos[0]��TrianglePos[1]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
    float scalar3_01 = d1 * d4 - d3 * d2;

    if (scalar3_01 <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
    {
        float t = d1 / (d1 - d3);
        *nearPos = DirectX::XMVectorAdd(TrianglePos[0], DirectX::XMVectorScale(Vec01, t));
        *nearTriPos1 = TrianglePos[0];
        *nearTriPos2 = TrianglePos[1];
        return false;
    }

    // point��TrianglePos[2]�̊O���̒��_�̈�ɂ��邩�`�F�b�N
    DirectX::XMVECTOR Vec2P = DirectX::XMVectorSubtract(Point, TrianglePos[2]);
    float d5 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec2P));
    float d6 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec2P));

    if (d6 >= 0.0f && d5 <= d6)
    {
        *nearPos = TrianglePos[2];
        *nearTriPos1 = TrianglePos[2];
        return false;
    }

    // point��TrianglePos[0]��TrianglePos[2]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
    float scalar3_02 = d5 * d2 - d1 * d6;

    if (scalar3_02 <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
    {
        float t = d2 / (d2 - d6);
        *nearPos = DirectX::XMVectorAdd(TrianglePos[0], DirectX::XMVectorScale(Vec02, t));
        *nearTriPos1 = TrianglePos[0];
        *nearTriPos2 = TrianglePos[2];
        return false;
    }

    // point��TrianglePos[1]��TrianglePos[2]�̊O���̕ӗ̈�ɂ��邩�`�F�b�N
    float scalar3_12 = d3 * d6 - d5 * d4;

    if (scalar3_12 <= 0.0f && d4 >= d3 && d5 >= d6)
    {
        float t = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        *nearPos = DirectX::XMVectorAdd(TrianglePos[1], DirectX::XMVectorScale(DirectX::XMVectorSubtract(TrianglePos[2], TrianglePos[1]), t));
        *nearTriPos1 = TrianglePos[1];
        *nearTriPos2 = TrianglePos[2];
        return false;
    }

    // �����܂ł���΁AnearPos�͎O�p�`�̓����ɂ���
    float denom = 1.0f / (scalar3_01 + scalar3_02 + scalar3_12);
    float t01 = scalar3_02 * denom;
    float t02 = scalar3_01 * denom;
    *nearPos = DirectX::XMVectorAdd(TrianglePos[0], DirectX::XMVectorAdd(DirectX::XMVectorScale(Vec01, t01), DirectX::XMVectorScale(Vec02, t02)));
    return true;
}

// ���ƎO�p�`�̌�������
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

// ���C�Ƌ��̌�������
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

// ���C�Ɖ~���̌�������
bool Collision::IntersectRayVsCylinder(
    const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end,
    const DirectX::XMVECTOR& startCylinder, const DirectX::XMVECTOR& endCylinder,
    float radius,
    DirectX::XMVECTOR* hitPos,
    float* hitDistance)
{
    // �Q�l�����F�Q�[���v���O���~���O�̂��߂̃��A���^�C���Փ˔��� P.194�`198������
    DirectX::XMVECTOR d = DirectX::XMVectorSubtract(endCylinder, startCylinder);
    DirectX::XMVECTOR m = DirectX::XMVectorSubtract(start, startCylinder);
    DirectX::XMVECTOR n = DirectX::XMVectorSubtract(end, start);

    float md = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, d));
    float nd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
    float dd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, d));

    // �������~���̂ǂ��炩�̒�ʂɑ΂��Ċ��S�ɊO���ɂ��邩�ǂ����𔻒�
    if (md < 0.0f && md + nd < 0.0f)
    {
        return false;	// �������~����startCylinder�̊O���ɂ���
    }
    if (md > dd && md + nd > dd)
    {
        return false;	// �������~����endCylinder�̊O���ɂ���
    }

    float nn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, n));
    float a = dd * nn - nd * nd;
    float k = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, m)) - radius * radius;
    float c = dd * k - md * md;

    // �������~���̎��ɑ΂��ĕ��s
    if (fabsf(a) < 0.0001f)		// �덷���o�₷���v�Z�Ȃ̂�臒l�͑傫�߁i0.0001f�j
    {
        if (c > 0.0f) return false;	// �����͉~���̊O��
        return true;
    }

    // �������~���̎��ɑ΂��ĕ��s�łȂ�
    // �~���̕\�ʂ�\���A�֐��������ƒ����̕������̉������߂Č���������s���B
    float mn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, n));
    float b = dd * mn - nd * md;
    float D = b * b - a * c;	// ���ʎ�

    if (D < 0) return false;	// ���������Ȃ��̂Ō������Ă��Ȃ�

    *hitDistance = (-b - sqrtf(D)) / a;
    if (*hitDistance < 0.0f)
    {
        *hitDistance = (-b + sqrtf(D)) / a;
        if (*hitDistance < 0.0f)
        {
            return false;	// ��_�������̊O���ɂ���������Ă��Ȃ�
        }
    }
    else if (*hitDistance > 1.0f)
    {
        *hitDistance = (-b + sqrtf(D)) / a;
        if (*hitDistance > 1.0f)
        {
            return false;	// ��_�������̊O���ɂ���������Ă��Ȃ�
        }
    }

    if (md + *hitDistance * nd < 0.0f)
    {
        // �~����startCylinder���̒�ʂ̊O�Ō���
        *hitDistance = -md / nd;

        // Dot(S(t) - startCylinder, S(t) - startCylinder) <= cylinder.rad * cylinder.rad �𖞂����ꍇ�A�������Ă���
        DirectX::XMVECTOR St = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(n, *hitDistance));
        St = DirectX::XMVectorSubtract(St, startCylinder);
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(St, St)) > radius * radius)
        {
            return false;
        }
    }
    else if (md + *hitDistance * nd > dd)
    {
        // �~����endCylinder���̒�ʂ̊O�Ō���
        *hitDistance = (dd - md) / nd;

        // Dot(S(t) - endCylinder, S(t) - endCylinder) <= cylinder.rad * cylinder.rad �𖞂����ꍇ�A�������Ă���
        DirectX::XMVECTOR St = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(n, *hitDistance));
        St = DirectX::XMVectorSubtract(St, endCylinder);
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(St, St)) > radius * radius)
        {
            return false;
        }
    }

    // �������~���̒�ʂƒ�ʂ̊ԂŌ������Ă���
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

    if (d > 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
    {
        if (fabs(d) > 1e-6f)	// ���s�m�F
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

// �X�t�B�A�L���X�g �ȈՔ�
bool Collision::IntersectSphereCastVsTriangleEST(
    const DirectX::XMVECTOR& start,
    const DirectX::XMVECTOR& end,
    const float radius,
    const DirectX::XMVECTOR trianglePos[3],
    HitResult& result)
{
    // �O�p�`�ƃX�^�[�g�ʒu�ɂ����鋅���������Ă���ꍇ�́Afalse�ŏI������
    if (IntersectSphereVsTriangle(start, radius, trianglePos, &result.position)) return false;

    DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(end, start);

    DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
    DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
    DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
    DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(start, end);
    float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

    if (d > 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
    {
        if (fabs(d) > 1e-6f)	// ���s�m�F
        {
            // ���C�̎n�_���O�p�`�̖@�������ɋ��\�ʂ̍ŒZ�_�Ɉړ�������
            DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVectorNegate(norm)), radius);
            DirectX::XMVECTOR startFix = DirectX::XMVectorAdd(start, fixVec);

            DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(startFix, trianglePos[0]);
            float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
            if (t < d)	// ���C�̌����ƒ����m�F
            {
                // ���C�ƎO�p�`���܂ޕ��ʂ̌�_�̎Z�o
                DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
                float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
                float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
                t /= d;
                result.position = DirectX::XMVectorAdd(startFix, DirectX::XMVectorScale(direction, t));

                if (v >= 0.0f && v <= d && w > 0.0f && v + w <= d)
                {
                    // ���C�ƎO�p�`���܂ޕ��ʂ̌�_���O�p�`�̓���
                    result.distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(direction)) * t;
                    result.normal = norm;
                    result.triangleVerts[0] = trianglePos[0];
                    result.triangleVerts[1] = trianglePos[1];
                    result.triangleVerts[2] = trianglePos[2];
                    return true;
                }
                else
                {
                    // ���C�ƎO�p�`���܂ޕ��ʂ̌�_���O�p�`�̊O��

                    // ��_�Ɉ�ԋ߂��O�p�`�����̓_�����߂�
                    DirectX::XMVECTOR nearPos = {};
                    ClosestPosPointTriangle(result.position, trianglePos, &nearPos);

                    // ��_�Ɉ�ԋ߂��O�p�`�����̓_����A�t�����̃��C�𔭎˂��A���C�̎n�_�ɂ��鋅�̂ƌ������邩�m�F
                    // ���������ꍇ�A��_�Ɉ�ԋ߂��O�p�`�����̓_���X�t�B�A�L���X�g�̎O�p�`�Ƃ̌�_�ɂȂ�
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

// �X�t�B�A�L���X�g ���S��
bool Collision::IntersectSphereCastVsTriangle(
    const DirectX::XMVECTOR& start,
    const DirectX::XMVECTOR& end,
    const float radius,
    const DirectX::XMVECTOR trianglePos[3],
    HitResult& result)
{
    // �O�p�`�ƃX�^�[�g�ʒu�ɂ����鋅���������Ă���ꍇ�́Afalse�ŏI������
    if (IntersectSphereVsTriangle(start, radius, trianglePos, &result.position)) return false;

    DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
    DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
    DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
    DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(start, end);
    float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));
    result.distance = FLT_MAX;

    if (d >= 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
    {
        // �O�p�`�̊e���_��@�������ɋ����a�����ړ�������
        DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(norm), radius);

        // �ړ���̎O�p�`�ƌ�������Ȃ�A���̎O�p�`�̓����ŃX�t�B�A�L���X�g���������邱�Ƃ��m��
        DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(start, DirectX::XMVectorAdd(trianglePos[0], fixVec));
        float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
        if (t >= 0.0f && t < d)		// ���C�̌����ƒ����m�F
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

        // �ʗ̈�Ō������Ȃ���΁A�{���m�C�̊e���_�̈�A�ӗ̈�Ō���������s���A�ŒZ�������Z�o����
        DirectX::XMVECTOR position = {};
        float distance = 0.0f;
        DirectX::XMVECTOR directionNormalize = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(end, start));

        // trianglePos[0] ���_�̈�̃`�F�b�N
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

        // trianglePos[1] ���_�̈�̃`�F�b�N
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

        // trianglePos[2] ���_�̈�̃`�F�b�N
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

        // trianglePos[0]-trianglePos[1] �ӗ̈�̃`�F�b�N
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

        // trianglePos[0]-trianglePos[2] �ӗ̈�̃`�F�b�N
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

        // trianglePos[1]-trianglePos[2] �ӗ̈�̃`�F�b�N
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


//�ȈՓI��AABB�̓����蔻��
bool Collision::AABBVsAABB(AABB first, AABB second, float lenge)
{
    //X�̌�������
    if (first.center.x - first.radius.x > second.center.x + second.radius.x + lenge)
    {
        return false;
    }
    if (first.center.x + first.radius.x < second.center.x - second.radius.x - lenge)
    {
        return false;
    }
    //Y�̌�������
    if (first.center.y - first.radius.y > second.center.y + second.radius.y + lenge)
    {
        return false;
    }
    if (first.center.y + first.radius.y < second.center.y - second.radius.y - lenge)
    {
        return false;
    }
    //Z�̌�������
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