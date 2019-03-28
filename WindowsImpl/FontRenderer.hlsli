#include "SpriteRenderer.hlsli"
#include "Blender.hlsli"

float FontShape(float sdf, float width, float r = 0.2)
{
	return clamp((sdf - (1 - width)) / r, 0.0f, 1.0f);
}

float4 FontPSFunc(SpritePSIn i) : SV_TARGET
{
	float sdf = SpriteTexture.Sample(SpriteSampler,i.TexCoord).r;
	float shape = FontShape(sdf, 0.6);

	return i.Color * float4(1,1,1,shape);
}
