#include <PostEffect.hlsli>

float4 PostEffectPS(PostEffectPSIn i) : SV_TARGET
{
	return float4(i.Coord.x,i.Coord.y,0,1);
}

technique11 PostEffect
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, PostEffectVSFunc()));
		SetPixelShader(CompileShader(ps_4_0, PostEffectPS()));
	}
};