struct HeightParams
{
    row_major float4x4 world_to_clip;
    uint2 texture_size;
    uint2 grid_size;
    float max_iterations;
    float height_scale;
    float2 padding;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D<float> iteration_tex : register(t0, space0);
ConstantBuffer<HeightParams> params : register(b0, space1);

uint2 triangle_corner(uint corner)
{
    uint2 corners[6] = {
        uint2(0u, 0u),
        uint2(1u, 0u),
        uint2(0u, 1u),
        uint2(0u, 1u),
        uint2(1u, 0u),
        uint2(1u, 1u),
    };

    return corners[corner];
}

VertexOutput main(uint vertex_id : SV_VertexID)
{
    uint2 grid_size = uint2(max(params.grid_size.x, 2u), max(params.grid_size.y, 2u));
    uint2 cells = grid_size - 1u;
    uint quad_id = vertex_id / 6u;
    uint corner_id = vertex_id % 6u;
    uint2 grid_coord = uint2(quad_id % cells.x, quad_id / cells.x) + triangle_corner(corner_id);
    float2 uv = float2(grid_coord) / float2(cells);

    uint2 texture_size = uint2(max(params.texture_size.x, 1u), max(params.texture_size.y, 1u));
    float2 texture_extent = float2(texture_size - 1u);
    uint2 texture_coord = uint2(uv * texture_extent + 0.5);
    float iteration = iteration_tex.Load(int3(int2(texture_coord), 0));

    float normalized_height = saturate(iteration / max(params.max_iterations, 1.0));
    float height = normalized_height * params.height_scale;
    float3 world_position = float3(uv.x * 2.0 - 1.0, height, 1.0 - uv.y * 2.0);

    VertexOutput output;
    output.position = mul(params.world_to_clip, float4(world_position, 1.0));
    output.uv = uv;
    return output;
}
