#include "psvsHeader.hlsli"

float4 main(PS_IN Input) : SV_TARGET
{
    return TextureData.Sample(SamplerData, Input.Tex);
}
