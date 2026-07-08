struct HeightColorParams
{
    float max_iterations;
    uint palette_size;
    uint2 padding;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float iteration : TEXCOORD0;
    float normalized_height : TEXCOORD1;
};

StructuredBuffer<uint> palette : register(t0, space2);
ConstantBuffer<HeightColorParams> params : register(b0, space3);

float4 unpack_palette_color(uint color)
{
    float red = float(color & 0xffu) / 255.0;
    float green = float((color >> 8) & 0xffu) / 255.0;
    float blue = float((color >> 16) & 0xffu) / 255.0;
    float alpha = float((color >> 24) & 0xffu) / 255.0;
    return float4(red, green, blue, alpha);
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
    float4 color = palette_color(input.iteration);
    float light = lerp(0.55, 1.0, saturate(input.normalized_height));
    return float4(color.rgb * light, color.a);
}
