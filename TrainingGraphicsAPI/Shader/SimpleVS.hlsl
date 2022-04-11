#include "psvsHeader.hlsli"

VS_OUT main( VS_IN input )
{
    VS_OUT output = (VS_OUT) 0;

    float4 localPos = input.pos;
    float4 worldPos = mul( World, localPos );
    float4 viewPos  = mul( View,  worldPos );
    float4 projPos  = mul(Projection,  viewPos );

    output.pos = projPos;
    output.col    = input.col;
    output.tex = input.tex;

    return output;
}