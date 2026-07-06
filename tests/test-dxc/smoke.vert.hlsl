float4 main(uint vertexID : SV_VertexID) : SV_Position
{
    float x = (vertexID == 1) ? 3.0 : -1.0;
    float y = (vertexID == 2) ? 3.0 : -1.0;
    return float4(x, y, 0.0, 1.0);
}
