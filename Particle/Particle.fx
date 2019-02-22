// �����˰���FontRenderer.hlsli�����Դ˴�������Ҫ����������ͷ�ļ�
//#include <SpriteRenderer.hlsli>
//#include <Blender.hlsli>

// ������ǰ����ͷ�ļ�������������Ⱦ��һЩ����
#include <FontRenderer.hlsli>

// ����ʹ��Ĭ�ϵľ���VS��GS
VertexShader SpriteVS = CompileShader(vs_4_0, SpriteVSFunc());
GeometryShader SpriteGS = CompileShader(gs_4_0, SpriteGSFunc());


// ������ʹ�õ�PS
float4 ParticlePSFunc(SpritePSIn i) : SV_TARGET
{
	float dis = distance(i.FragPosition,float2(0.5,0.5)) + 0.5;
	float col = clamp(1 - dis,0,1) * 2;
	col = col * col * col * col;
	return float4(1,1,1, col) * i.Color;
}

// ��������ʹ�õ�PS
PixelShader ParticlePS = CompileShader(ps_4_0, ParticlePSFunc());

// ������Ⱦ����
technique11 Particle
{
	pass P0
	{
		// ���û��ģʽ
		SetBlendState(AddtiveAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		// ���ö�����ɫ����������ɫ���ͼ�����ɫ��
		SetVertexShader(SpriteVS);
		SetPixelShader(ParticlePS);
		SetGeometryShader(SpriteGS);
	}
};

// ���ڰ�ɫ�����ڴ˴������������Դ˴���Ϊ��ɫ���壬��Ϊ��Ⱦ�����õ�PS
float4 DisplayFontPS(SpritePSIn i) : SV_TARGET
{
	float sdf = SpriteTexture.Sample(SpriteSampler,i.TexCoord).r;
	float shape = FontShape(sdf, 0.6);

	return float4(0,0,1,shape);
}

// ������Ⱦ��������Ҫ��PS
PixelShader FontPS = CompileShader(ps_4_0, DisplayFontPS());

// ������Ⱦ������������Ⱦ���½ǵ���������
technique11 FontRendering
{
	pass P0
	{
		// ���û��ģʽ
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		// ���ö�����ɫ����������ɫ���ͼ�����ɫ��
		SetVertexShader(SpriteVS);
		SetPixelShader(FontPS);
		SetGeometryShader(SpriteGS);
	}
};
