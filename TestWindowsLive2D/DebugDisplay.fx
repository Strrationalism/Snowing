#include <FontRenderer.hlsli>

technique11 DebugDisplay
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0,SpriteVSFunc()));
		SetPixelShader(CompileShader(ps_4_0, FontPSFunc()));
		SetGeometryShader(CompileShader(gs_4_0, SpriteGSFunc()));
	}
};
