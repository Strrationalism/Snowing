
struct VSIn
{
	float2 Input : POSITION;
};

struct GSIn
{
	float2 Input : POSITION;
	float4 Pos : SV_POSITION;
};

GSIn VS(VSIn i)
{
	GSIn o;
	o.Input = i.Input;
	o.Pos = float4(i.Input.x, i.Input.y, 0, 1);
	return o;
}

struct GSOut
{
	float2 OutputData : OUTPUT;
	float4 Pos : SV_POSITION;
};

[maxvertexcount(1)]
void StreamOutDemo(
	point GSIn In[1],
	inout PointStream<GSOut> OutputStream)
{
	GSOut o;
	o.OutputData = In[0].Input - float2(0.01,0.01);
	o.Pos = In[0].Pos;
	OutputStream.Append(o);
};

float4 PS() : SV_TARGET
{
	return float4(1,1,1,1);
}

technique11 StreamOutDemoTech
{
	pass P0
	{
		SetPixelShader(CompileShader(ps_4_0, PS()));
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(
			ConstructGSWithSO(
				CompileShader(gs_4_0, StreamOutDemo()),
				"0:OUTPUT.xy"
			)
		);
	}
};