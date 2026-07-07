struct BlitParams
{
    uint max_iterations;
    uint palette_size;
    uint2 padding;
};

Texture2D<uint> iteration_tex : register(t0, space2);
StructuredBuffer<uint> palette : register(t1, space2);
ConstantBuffer<BlitParams> params : register(b0, space3);

float4 main(float4 position : SV_Position) :
    SV_Target0
{
    uint iteration = iteration_tex.Load(int3(int2(position.xy), 0));
    if (iteration >= params.max_iterations)
    {
        return float4(0.0, 0.0, 0.0, 1.0);
    }

    uint color = palette[iteration % params.palette_size];
    float red = float(color & 0xffu) / 255.0;
    float green = float((color >> 8) & 0xffu) / 255.0;
    float blue = float((color >> 16) & 0xffu) / 255.0;
    float alpha = float((color >> 24) & 0xffu) / 255.0;
    return float4(red, green, blue, alpha);
}
