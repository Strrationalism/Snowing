#include <FontRenderer.hlsli>

VertexShader vs = CompileShader(vs_4_0, SpriteVSFunc());
PixelShader ps = CompileShader(ps_4_0, FontPSFunc());
GeometryShader gs = CompileShader(gs_4_0, SpriteGSFunc());

technique11 Font
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(vs);
		SetPixelShader(ps);
		SetGeometryShader(gs);
	}
};
