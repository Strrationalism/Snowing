#include <Snowing.h>

using namespace Snowing;

int main()
{
	// ��������ʵ��
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(
		L"TextRendering",
		{ 800,600 },
		true);

	// ������ɫ��
	Graphics::Effect effect
	{
		LoadAsset("SpriteShader.cso")
	};

	// ���ؾ�����Ⱦ����
	auto tech = effect.LoadTechnique(
		"SpriteRendering",
		Graphics::Sprite::DataLayout);

	// ����ϵ����Ļ����Ϊ(0,0)����Ļ��800����600��
	constexpr Math::Coordinate2DCenter coord
	{
		{ 0.0f, 0.0f },
		{ 800.0f, 600.0f }
	};

	// �����б�
	Graphics::SpriteSheet spriteSheet;

	// ���ؾ���ͼ���������浽image�������б������spriteSheet
	auto image = Graphics::LoadSpriteAndSpriteSheet(
		LoadAsset("Sprite.ctx"),
		spriteSheet);

	// ���;����б�GPU����
	auto spriteSheetGpu = 
		Graphics::SpriteRenderer<>::MakeGPUSpriteSheet(
			spriteSheet);

	// ���㻺��
	auto vb = Graphics::SpriteRenderer<>::MakeGPUVertexBuffer();

	// ������Ⱦ��
	Graphics::SpriteRenderer<> renderer
	{
		&Graphics::Device::MainContext(),	// ����ͼ����
		&tech,								// ������Ⱦ����
		coord,								// ����ϵ
		&image,								// ����
		&spriteSheetGpu,					// GPU�ϵľ����б�
		&vb									// ���㻺��
	};

	// ����һ������
	Graphics::Sprite sp;

	// �����ڶ�������
	Graphics::Sprite sp2;

	//���õڶ��������һЩ����
	sp2.Position = { 0,0 };	// ����
	sp2.Center = { 0,0 };	// ���ĵ��ھ�������Ͻǣ����½�Ϊ(1,1)
	sp2.ImageID = 2;		// ʹ�þ���ͼ���ĵ�2�ž���
	
	// ������ѭ��
	Engine::Get().Run([&] {

		// ��ʱ��ӵ��ڶ����������ת�Ƕ��ϣ�ʹ������ʱ����ת
		sp2.Rotating += Engine::Get().DeltaTime();

		// ����
		Engine::Get().Draw([&] {

			// ���������
			Graphics::Device::MainContext().ClearRenderTarget(
				Graphics::Device::MainRenderTarget());

			// ����������Ϊ��ǰ������Ⱦ�Ļ���
			Graphics::Device::MainContext().SetRenderTarget(
				&Graphics::Device::MainRenderTarget());

			// ��sp��sp2���Ƶ�renderer�ľ��黺��
			renderer.DrawToSpriteBuffer(sp);
			renderer.DrawToSpriteBuffer(sp2);

			// ��ɻ��ƣ������黺�淢�͵�GPU
			renderer.FlushSpriteBuffer();
		});
	});
}