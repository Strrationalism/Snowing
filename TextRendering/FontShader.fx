#include <FontRenderer.hlsli>

// 编译使用默认的精灵VS、GS和字体使用的PS
VertexShader SpriteVS = CompileShader(vs_4_0, SpriteVSFunc());
GeometryShader SpriteGS = CompileShader(gs_4_0, SpriteGSFunc());
PixelShader FontPS = CompileShader(ps_4_0, FontPSFunc());

technique11 FontRendering
{
	pass P0
	{
		// 设置混合模式
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		// 设置顶点着色器、像素着色器和几何着色器
		SetVertexShader(SpriteVS);
		SetPixelShader(FontPS);
		SetGeometryShader(SpriteGS);
	}
};
