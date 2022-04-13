struct VS_IN
{
    float4 Pos : POSITION0;
    float4 Col : COLOR0;
    float3 Nor : NORMAL0;
    float2 Tex : TEXCOORD0;
};
 
struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
    float3 Nor : NORMAL0;
    float2 Tex : TEXCOORD0;
};

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
    float3 Nor : NORMAL0;
    float2 Tex : TEXCOORD0;
};

cbuffer ConstantBuffer
{
    float4x4 World;         // ワールド変換行列
    float4x4 View;          // ビュー変換行列
    float4x4 Projection;    // 透視射影変換行列
    float4   Light;         // ライトベクトル
}

Texture2D    TextureData : register(t0);
SamplerState SamplerData : register(s0);
