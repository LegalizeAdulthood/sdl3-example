struct HeightColorParams
{
    float max_iterations;
    uint palette_size;
    uint2 padding;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D<float> iteration_tex : register(t0, space2);
StructuredBuffer<uint> palette : register(t1, space2);
ConstantBuffer<HeightColorParams> params : register(b0, space3);

float4 unpack_palette_color(uint color)
{
    float red = float(color & 0xffu) / 255.0;
    float green = float((color >> 8) & 0xffu) / 255.0;
    float blue = float((color >> 16) & 0xffu) / 255.0;
    float alpha = float((color >> 24) & 0xffu) / 255.0;
    return float4(red, green, blue, alpha);
}

float load_iteration(float2 uv)
{
    uint width;
    uint height;
    iteration_tex.GetDimensions(width, height);

    uint2 texture_size = uint2(max(width, 1u), max(height, 1u));
    float2 texture_extent = float2(texture_size - 1u);
    uint2 texture_coord = uint2(uv * texture_extent + 0.5);
    return iteration_tex.Load(int3(int2(texture_coord), 0));
}

float4 palette_color(float iteration)
{
    if (params.palette_size == 0u || iteration >= params.max_iterations)
    {
        return float4(0.0, 0.0, 0.0, 1.0);
    }

    float palette_size = float(params.palette_size);
    float wrapped = iteration - floor(iteration / palette_size) * palette_size;
    float base = floor(wrapped);
    uint first_index = uint(base);
    uint second_index = (first_index + 1u) % params.palette_size;
    return lerp(
        unpack_palette_color(palette[first_index]), unpack_palette_color(palette[second_index]), wrapped - base);
}

float4 main(VertexOutput input) :
    SV_Target0
{
    float iteration = load_iteration(input.uv);
    float4 color = palette_color(iteration);
    float normalized_height = saturate(iteration / max(params.max_iterations, 1.0));
    float light = lerp(0.55, 1.0, normalized_height);
    return float4(color.rgb * light, color.a);
}
