#include "Particle.hlsli"

StructuredBuffer<particle> particle_buffer : register(t9);

[maxvertexcount(4)]
void main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    //パーティクル位置から正方形を生成
    const float3 billboard[] =
    {
        float3(-1.0f, -1.0f, 0.0f),
		float3(-1.0f, +1.0f, 0.0f),
		float3(+1.0f, -1.0f, 0.0f),
		float3(+1.0f, +1.0f, 0.0f),
    };
    const float2 texcoords[] =
    {
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
        float2(0.0f, 1.0f),
    };
    
    particle p = particle_buffer[input[0].vertex_id];
    
    float3 Z = normalize(p.normal.xyz);
    float3 X = normalize(cross(Z, float3(0, 1, 0)));
    float3 Y = normalize(cross(Z, X));
    row_major float3x3 R = { X, Y, Z };

    //アニメーションやタイル位置の指定があればtexcoordの位置を変更
    float plaX = 1.0 / float(p.tiles.x);
    float plaY = 1.0 / float(p.tiles.y);
    int ti = p.use_tile;
    uint momeX = ti % p.tiles.x;
    uint momeY = ti / p.tiles.x;
    
	[unroll]
    for (uint vertex_index = 0; vertex_index < 4; ++vertex_index)
    {
        GS_OUT element;
        element.draw_type = p.draw_type;
        
        float3 corner_pos = billboard[vertex_index] * particle_size * p.length_shrink.x * p.size;

        element.position = mul(float4(p.position.xyz + mul(corner_pos, R), 1), view_projection);
        element.color = p.color;
        
        
        element.texcoord = texcoords[vertex_index];
        element.texcoord.x = momeX * plaX + element.texcoord.x * plaX;
        
        output.Append(element);
    }

    output.RestartStrip();
}