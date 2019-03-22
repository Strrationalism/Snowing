#include "pch.h"
#include <L2DDevice.h>
#include <L2DModelAsset.h>
#include <L2DModel.h>
#include <L2DBreath.h>
#include <L2DBlink.h>

void RenderModel(const char* modelHome, const char* modelJson)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Rendering.RenderModel", { 1024,768 }, true);

	// 启动Live2D设备
	Live2D::Device device;

	// 加载Live2D资源
	Live2D::ModelAsset ass{ modelHome,modelJson };

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());

	// 根场景
	Scene::Group<> group;
	group.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget(),
		Math::Vec4f{ 0,0,0,0 });

	group.Emplace<Scene::VirtualTask>(5.0f,[] {Engine::Get().Exit(); });

	// 一个Live2D对象
	auto model = group.Emplace<Live2D::Model>(&Graphics::Device::MainContext(),&ass, 1024.0f / 768.0f);

	// 添加Live2D对象的呼吸效果
	group.Emplace<Live2D::Breath>(model, Live2D::Breath::Params{});

	// 添加Live2D对象的眨眼效果
	group.Emplace<Live2D::Blink>(model, 1.0f);

	Snowing::Engine::Get().RunObject(group);
}

TEST(Rendering, Hiyori)
{
	RenderModel("Live2D/Hiyori/", "Hiyori.model3.json");
}

TEST(Rendering, Haru)
{
	RenderModel("Live2D/Haru/", "Haru.model3.json");
}

TEST(Rendering, Mark)
{
	RenderModel("Live2D/Mark/", "Mark.model3.json");
}
