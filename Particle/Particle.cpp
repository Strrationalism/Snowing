#include <Snowing.h>
#include "Fire.h"
using namespace Snowing;


int main()
{
	// 创建引擎实例
	auto [window, device, engine] = PlatformImpls::WindowsImpl::MakeEngine(
		L"Particle",	// 窗口标题
		{ 800,600 },	// 窗口大小
		true);			// 使用窗口

	// 坐标系
	Math::Coordinate2DCenter coord
	{
		{ 0.0f,0.0f },		// 屏幕中心坐标为0,0
		{ 800.0f,600.0f }	// 屏幕大小为800,600
	};

	Snowing::Graphics::Effect effect{
		LoadAsset("Particle.cso")
	};

	Snowing::Scene::Group<> scene;

	Fire *fire = scene.Emplace<Fire>(&effect, coord);

	// 加载各种Debug显示器
	const Graphics::Font fnt =
		Graphics::LoadFont(
			LoadAsset("Font-chs.fnt"));

	auto fontRenderingTech = effect.LoadTechnique("FontRendering", Graphics::Sprite::DataLayout);

	scene.Emplace<Scene::Debug::DebugDisplay>(
		&fontRenderingTech,
		&fnt,
		L"粒子数量",
		[fire] { return std::to_wstring(fire->GetParticlesCount()); }
	);

	scene.Emplace<Scene::Debug::DebugDisplay>(
		&fontRenderingTech,
		&fnt,
		L"FPS",
		Scene::Debug::DebugDisplay::FPSGetter
	);

	scene.Emplace<Scene::Debug::DebugDisplay>(
		&fontRenderingTech,
		&fnt,
		L"帧时间",
		Scene::Debug::DebugDisplay::FrameTimeGetter
	);

	engine.RunObject(scene);

	return 0;
}
