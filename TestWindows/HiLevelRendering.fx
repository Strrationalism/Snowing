#include <FontRenderer.hlsli>

VertexShader SpriteVS = CompileShader(vs_4_0, SpriteVSFunc());
PixelShader SpritePS = CompileShader(ps_4_0, SpritePSFunc());
GeometryShader SpriteGS = CompileShader(gs_4_0, SpriteGSFunc());


float4 MultiPassTest(SpritePSIn i) : SV_TARGET
{
	return float4(1.0,1.0,1.0,0.1);
}

float4 NoTexSpriteTest(SpritePSIn i) : SV_TARGET
{
	return float4(i.FragPosition.x,i.FragPosition.y,0,1);
}



PixelShader MultiPassTestPS = CompileShader(ps_4_0, MultiPassTest());
PixelShader NoTexSpriteTestPS = CompileShader(ps_4_0, NoTexSpriteTest());

technique11 SpriteTest
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(SpriteVS);
		SetPixelShader(SpritePS);
		SetGeometryShader(SpriteGS);
	}

	pass P1
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(SpriteVS);
		SetPixelShader(MultiPassTestPS);
		SetGeometryShader(SpriteGS);
	}
};

technique11 EffectOnly
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(SpriteVS);
		SetPixelShader(NoTexSpriteTestPS);
		SetGeometryShader(SpriteGS);
	}
};



float4 FontLightingPSFunc(SpritePSIn i) : SV_TARGET
{
	float sdf = SpriteTexture.Sample(SpriteSampler,i.TexCoord).r;
	float shape = FontShape(sdf, 0.65);

	const float3 a = float3(185, 195, 199) / float3(255,255,255);
	const float3 b = float3(44, 62, 80) / float3(255, 255, 255);
	float3 innerCol = lerp(a, b, i.FragPosition.y);

	float4 finalColor = float4(0, 0, 0, 0);
	finalColor = float4(i.FragPosition.x, 0, i.FragPosition.y, sdf * sdf * 2) * 2.0;
	finalColor = NormalAlphaBlendFunc(float4(1, 1, 1, shape), finalColor);

	return i.Color * finalColor;
}


float FontShape2(float sdf, float width)
{
	return clamp((sdf - (1 - width)) / 0.3f, 0.0f, 1.0f / 0.3f);
}


float4 BasicFontPSFunc(SpritePSIn i) : SV_TARGET
{
	float sdf = SpriteTexture.Sample(SpriteSampler,i.TexCoord).r;
	float shape = FontShape2(sdf, 0.8);

	return float4(1, 1, 1, shape);
}


PixelShader FontPS1 = CompileShader(ps_4_0, FontLightingPSFunc());
PixelShader FontPS2 = CompileShader(ps_4_0, BasicFontPSFunc());


technique11 FontTestLighting
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(SpriteVS);
		SetPixelShader(FontPS1);
		SetGeometryShader(SpriteGS);
	}
};

technique11 FontTestBasic
{
	pass P0
	{
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(SpriteVS);
		SetPixelShader(FontPS2);
		SetGeometryShader(SpriteGS);
	}
};

