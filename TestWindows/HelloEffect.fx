
Texture2D tex;

cbuffer e1
{
	float coll;
};

SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float colMul;

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 OrgPos : POSITION;
};

VS_OUTPUT VS(float2 pos : POSITION,float4 col : COLOR)
{
    VS_OUTPUT o;
    o.Position = float4(pos.x,pos.y,0,1);
	o.Color = col;
	o.OrgPos = pos;
    return o;
}

struct PS_OUTPUT
{
    float4 RGBColor : SV_Target; // Pixel color
};

PS_OUTPUT PS(VS_OUTPUT i)
{
    PS_OUTPUT o;
	o.RGBColor = i.Color * colMul;
	o.RGBColor *= tex.Sample(MeshTextureSampler,i.OrgPos);
    return o;
}

fxgroup MyGroup
{
    technique11 HelloEffect
    {
        pass P0
        {
            SetVertexShader(CompileShader(vs_4_0, VS()));
            SetGeometryShader(NULL);
            SetPixelShader(CompileShader(ps_4_0, PS()));
        }
    }
}

technique11 Hello
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}