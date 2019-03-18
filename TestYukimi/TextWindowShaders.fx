#include <FontRenderer.hlsli>

VertexShader vs = CompileShader(vs_4_0,SpriteVSFunc());
GeometryShader gs = CompileShader(gs_4_0, SpriteGSFunc());
PixelShader basicFont = CompileShader(ps_4_0,FontPSFunc());

technique11 BasicFont
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(vs);
		SetPixelShader(basicFont);
		SetGeometryShader(gs);
	}
};