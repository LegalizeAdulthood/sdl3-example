struct MandelParams
{
    float4 view;
    float4 constants;
    float4 potential;
    int4 image;
    int4 periodicity;
};

RWTexture2D<float> iteration_tex : register(u0, space1);
RWTexture2D<float> potential_tex : register(u1, space1);
ConstantBuffer<MandelParams> params : register(b0, space2);

float continuous_iteration(int iter, float norm)
{
    float log_radius = log(sqrt(norm));
    if (log_radius <= 0.0)
    {
        return float(iter);
    }

    return max(float(iter) + 1.0 - log2(log_radius), 0.0);
}

float potential_height(int iter, float norm)
{
    float radius = max(sqrt(norm), 1.000001);
    float potential = log(radius) * exp2(-float(iter));
    return saturate(log2(1.0 + potential * params.potential.y) * params.potential.z);
}

[numthreads(16, 16, 1)] void main(uint3 tid : SV_DispatchThreadID)
{
    if (tid.x >= uint(params.image.x) || tid.y >= uint(params.image.y))
    {
        return;
    }

    float cx = params.view.x + float(tid.x) * params.view.z;
    float cy = params.view.y + float(tid.y) * params.view.w;

    float x = cx + params.constants.x;
    float y = cy + params.constants.y;
    float saved_x = 0.0;
    float saved_y = 0.0;
    int saved_and = params.image.w;
    int saved_incr = 1;
    bool iteration_written = false;

    for (int iter = 1; iter < params.image.z; ++iter)
    {
        float x2 = x * x;
        float y2 = y * y;
        float xy = x * y;

        x = x2 - y2 + cx;
        y = 2.0 * xy + cy;

        float norm = x * x + y * y;
        if (!iteration_written && norm >= params.constants.z)
        {
            iteration_tex[tid.xy] = continuous_iteration(iter, norm);
            iteration_written = true;
        }

        if (norm >= params.potential.x)
        {
            if (!iteration_written)
            {
                iteration_tex[tid.xy] = continuous_iteration(iter, norm);
            }
            potential_tex[tid.xy] = potential_height(iter, norm);
            return;
        }

        if (!iteration_written && params.periodicity.y != 0)
        {
            if ((iter & saved_and) == 0)
            {
                saved_x = x;
                saved_y = y;
                --saved_incr;

                if (saved_incr == 0)
                {
                    saved_and = (saved_and << 1) + 1;
                    saved_incr = params.periodicity.x;
                }
            }
            else if (abs(saved_x - x) < params.constants.w && abs(saved_y - y) < params.constants.w)
            {
                iteration_tex[tid.xy] = float(params.image.z);
                potential_tex[tid.xy] = 0.0;
                return;
            }
        }
    }

    if (!iteration_written)
    {
        iteration_tex[tid.xy] = float(params.image.z);
    }
    potential_tex[tid.xy] = 0.0;
}
