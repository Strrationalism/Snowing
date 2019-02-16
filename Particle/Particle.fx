#include <SpriteRenderer.hlsli>
#include <Blender.hlsli>

// 编译使用默认的精灵VS、GS
VertexShader SpriteVS = CompileShader(vs_4_0, SpriteVSFunc());
GeometryShader SpriteGS = CompileShader(gs_4_0, SpriteGSFunc());



float4 ParticlePSFunc(SpritePSIn i) : SV_TARGET
{
	float dis = distance(i.FragPosition,float2(0.5,0.5)) + 0.5;
	float col = clamp(1 - dis,0,1) * 2;
	col = col * col * col * col;
	return float4(1,1,1, col) * i.Color;
}

PixelShader ParticlePS = CompileShader(ps_4_0, ParticlePSFunc());

technique11 Particle
{
	pass P0
	{
		// 设置混合模式
		SetBlendState(AddtiveAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		// 设置顶点着色器、像素着色器和几何着色器
		SetVertexShader(SpriteVS);
		SetPixelShader(ParticlePS);
		SetGeometryShader(SpriteGS);
	}
};
