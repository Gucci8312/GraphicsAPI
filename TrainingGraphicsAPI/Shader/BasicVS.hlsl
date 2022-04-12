#include "psvsHeader.hlsli"
 
VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(input.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);    
    
    output.col = input.col;
    output.tex = input.tex;
    return output;
}