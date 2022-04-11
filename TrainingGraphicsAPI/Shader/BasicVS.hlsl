#include "psvsHeader.hlsli"
 
VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float3 nor;
    float col;
    
    output.pos = mul(input.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    
    nor = mul(input.nor, World).xyz;
    nor = normalize(nor);

    col = saturate(dot(nor, (float3) Light));
    col = col * 0.5f;
    
    output.col = float4(col, col, col, 1.0f);
    output.tex = input.tex;
    return output;
}