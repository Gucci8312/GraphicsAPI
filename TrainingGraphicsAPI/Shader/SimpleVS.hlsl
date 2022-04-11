#include "psvsHeader.hlsli"


cbuffer ConstantBuffer
{
    float4x4 World; //ワールド変換行列
    float4x4 View; //ビュー変換行列
    float4x4 Projection; //透視射影変換行列
    float4 Light;
}

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