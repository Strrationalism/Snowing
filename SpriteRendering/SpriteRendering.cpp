#include <Snowing.h>

using namespace Snowing;

int main()
{
	// 创建引擎实例
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(
		L"TextRendering",
		{ 800,600 },
		true,
		{ true });

	// 加载着色器
	Graphics::Effect effect
	{
		LoadAsset("SpriteShader.cso")
	};

	// 加载精灵渲染技术
	auto tech = effect.LoadTechnique(
		"SpriteRendering",
		Graphics::Sprite::DataLayout);

	// 坐标系，屏幕中心为(0,0)，屏幕宽800，高600。
	constexpr Math::Coordinate2DCenter coord
	{
		{ 0.0f, 0.0f },
		{ 800.0f, 600.0f }
	};

	// 精灵列表
	Graphics::SpriteSheet spriteSheet;

	// 加载精灵图集，纹理保存到image，精灵列表输出到spriteSheet
	auto image = Graphics::LoadSpriteAndSpriteSheet(
		LoadAsset("Sprite.ctx"),
		spriteSheet);

	// 传送精灵列表到GPU缓存
	auto spriteSheetGpu = 
		Graphics::SpriteRenderer<>::MakeGPUSpriteSheet(
			spriteSheet);

	// 顶点缓存
	auto vb = Graphics::SpriteRenderer<>::MakeGPUVertexBuffer();

	// 精灵渲染器
	Graphics::SpriteRenderer<> renderer
	{
		&Graphics::Device::MainContext(),	// 主绘图环境
		&tech,								// 精灵渲染技术
		coord,								// 坐标系
		&image,								// 纹理
		&spriteSheetGpu,					// GPU上的精灵列表
		&vb									// 顶点缓存
	};

	// 创建一个精灵
	Graphics::Sprite sp;

	// 创建第二个精灵
	Graphics::Sprite sp2;

	//设置第二个精灵的一些参数
	sp2.Position = { 0,0 };	// 坐标
	sp2.Center = { 0,0 };	// 中心点在精灵的左上角，右下角为(1,1)
	sp2.ImageID = 2;		// 使用精灵图集的第2号精灵
	
	// 启动主循环
	Engine::Get().Run([&] {

		// 把时间加到第二个精灵的旋转角度上，使其随着时间旋转
		sp2.Rotating += Engine::Get().DeltaTime();

		// 绘制
		Engine::Get().Draw([&] {

			// 清空主画布
			Graphics::Device::MainContext().ClearRenderTarget(
				Graphics::Device::MainRenderTarget());

			// 设置主画布为当前正在渲染的画布
			Graphics::Device::MainContext().SetRenderTarget(
				&Graphics::Device::MainRenderTarget());

			// 将sp和sp2绘制到renderer的精灵缓存
			renderer.DrawToSpriteBuffer(sp);
			renderer.DrawToSpriteBuffer(sp2);

			// 完成绘制，将精灵缓存发送到GPU
			renderer.FlushSpriteBuffer();
		});
	});
}