struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float3 nor : NORMAL0;
    float2 tex : TEXCOORD0;
};
 
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float3 nor : NORMAL0;
    float2 tex : TEXCOORD0;
};

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
    float3 nor : NORMAL0;
    float2 tex : TEXCOORD0;
};

cbuffer ConstantBuffer
{
    float4x4 World; //���[���h�ϊ��s��
    float4x4 View; //�r���[�ϊ��s��
    float4x4 Projection; //�����ˉe�ϊ��s��
    float4 Light;
}