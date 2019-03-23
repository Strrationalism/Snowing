#include "pch.h"
#include <future>
#include <L2DDevice.h>
#include <L2DModelAsset.h>
#include <L2DModel.h>
#include <L2DBreath.h>
#include <L2DBlink.h>
#include <L2DPhysics.h>

void RenderModel(const char* modelHome, const char* modelJson,float height)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Rendering.RenderModel", { 1920 / 3 * 2,1080 / 3 * 2 }, true);

	// 启动Live2D设备
	Live2D::Device device;

	// 异步加载Live2D资源
	auto assFut = std::async([modelHome, modelJson]{
		return std::make_unique<Live2D::ModelAsset>( modelHome,modelJson );
	});
	assFut.wait();
	auto ass = assFut.get();

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());


	// 屏幕坐标系
	Math::Coordinate2DRect Coord
	{
		{-512,-384},
		{512,384}
	};

	// 根场景
	Scene::Group<> group;
	group.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget(),
		Math::Vec4f{ 0,0,0,0 });

	group.Emplace<Scene::VirtualTask>(5.0f,[] {Engine::Get().Exit(); });

	// 一个Live2D对象
	auto model = group.Emplace<Live2D::Model>(&Graphics::Device::MainContext(),ass.get(), 1920.0f / 1080.0f);
	auto model2 = group.Emplace<Live2D::Model>(&Graphics::Device::MainContext(), ass.get(), 1920.0f / 1080.0f);

	// 添加Live2D对象的呼吸效果
	group.Emplace<Live2D::Breath>(model, Live2D::Breath::Params{});
	group.Emplace<Live2D::Breath>(model2, Live2D::Breath::Params{});

	// 添加Live2D对象的眨眼效果
	group.Emplace<Live2D::Blink>(model, 1.0f);
	group.Emplace<Live2D::Blink>(model2, 1.0f);

	// 添加物理效果
	group.Emplace<Live2D::Physics>(model);

	model->SetTranslate({ 256.0f,-0.15f*384.0f }, Coord);
	model2->SetTranslate({ -256.0f,-height * 384.0f }, Coord);
	model2->SetScale({ 4,4 });

	Snowing::Engine::Get().RunObject(group);
}

TEST(Rendering, Hiyori)
{
	RenderModel("Live2D/Hiyori/", "Hiyori.model3.json",1.7f);
}

TEST(Rendering, Haru)
{
	RenderModel("Live2D/Haru/", "Haru.model3.json",2.7f);
}

TEST(Rendering, Mark)
{
	RenderModel("Live2D/Mark/", "Mark.model3.json",0.65f);
}
