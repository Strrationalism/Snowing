// 由于了包含FontRenderer.hlsli，所以此处不再需要包含这两个头文件
//#include <SpriteRenderer.hlsli>
//#include <Blender.hlsli>

// 包含了前两个头文件和用于字体渲染的一些功能
#include <FontRenderer.hlsli>

// 编译使用默认的精灵VS、GS
VertexShader SpriteVS = CompileShader(vs_4_0, SpriteVSFunc());
GeometryShader SpriteGS = CompileShader(gs_4_0, SpriteGSFunc());


// 粒子所使用的PS
float4 ParticlePSFunc(SpritePSIn i) : SV_TARGET
{
	float dis = distance(i.FragPosition,float2(0.5,0.5)) + 0.5;
	float col = clamp(1 - dis,0,1) * 2;
	col = col * col * col * col;
	return float4(1,1,1, col) * i.Color;
}

// 编译粒子使用的PS
PixelShader ParticlePS = CompileShader(ps_4_0, ParticlePSFunc());

// 粒子渲染技术
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

// 由于白色字体在此处看不见，所以此处改为蓝色字体，此为渲染字体用的PS
float4 DisplayFontPS(SpritePSIn i) : SV_TARGET
{
	float sdf = SpriteTexture.Sample(SpriteSampler,i.TexCoord).r;
	float shape = FontShape(sdf, 0.6);

	return float4(0,0,1,shape);
}

// 编译渲染字体所需要的PS
PixelShader FontPS = CompileShader(ps_4_0, DisplayFontPS());

// 字体渲染技术，用于渲染右下角的粒子数量
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
