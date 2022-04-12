#include "psvsHeader.hlsli"

VS_OUT main( VS_IN input )
{
    VS_OUT Output = (VS_OUT) 0;

    float4 localPos = input.Pos;
    float4 worldPos = mul( World, localPos );
    float4 viewPos  = mul( View,  worldPos );
    float4 projPos  = mul(Projection,  viewPos );

    Output.Pos = projPos;
    Output.Col = input.Col;
    Output.Tex = input.Tex;

    return Output;
}