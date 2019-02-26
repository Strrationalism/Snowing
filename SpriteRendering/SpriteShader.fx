#include <SpriteRenderer.hlsli>
#include <Blender.hlsli>

// 编译三个着色器
VertexShader vs = CompileShader(vs_4_0, SpriteVSFunc());
GeometryShader gs = CompileShader(gs_4_0, SpriteGSFunc());
PixelShader ps = CompileShader(ps_4_0, SpritePSFunc());

// 精灵渲染技术
technique11 SpriteRendering
{
	// 第0趟渲染
	pass P0
	{
		// 设置混合模式
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		// 设置三个着色器
		SetVertexShader(vs);
		SetGeometryShader(gs);
		SetPixelShader(ps);
	}
};
