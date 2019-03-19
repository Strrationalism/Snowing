#include <FontRenderer.hlsli>

float4 FontLightingPSFunc(SpritePSIn i) : SV_TARGET
{
	float sdf = SpriteTexture.Sample(SpriteSampler,i.TexCoord).r;
	float shape = FontShape(sdf, 0.65);

	const float3 a = float3(185, 195, 199) / float3(255,255,255);
	const float3 b = float3(44, 62, 80) / float3(255, 255, 255);
	float3 innerCol = lerp(a, b, i.FragPosition.y);

	float4 finalColor = float4(0, 0, 0, 0);
	finalColor = float4(i.FragPosition.x, 0, i.FragPosition.y, sdf * sdf * 2) * 2.0;

	return i.Color* finalColor;
}

VertexShader vs = CompileShader(vs_4_0,SpriteVSFunc());
GeometryShader gs = CompileShader(gs_4_0, SpriteGSFunc());
PixelShader basicFont = CompileShader(ps_4_0, FontLightingPSFunc());

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