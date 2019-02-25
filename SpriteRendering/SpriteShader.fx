#include <SpriteRenderer.hlsli>
#include <Blender.hlsli>

// ����������ɫ��
VertexShader vs = CompileShader(vs_4_0, SpriteVSFunc());
GeometryShader gs = CompileShader(gs_4_0, SpriteGSFunc());
PixelShader ps = CompileShader(ps_4_0, SpritePSFunc());

// ������Ⱦ����
technique11 SpriteRendering
{
	// ��0����Ⱦ
	pass P0
	{
		// ���û��ģʽ
		SetBlendState(NormalAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		// ����������ɫ��
		SetVertexShader(vs);
		SetGeometryShader(gs);
		SetPixelShader(ps);
	}
};
