#include "pch.h"
#include <L2DDevice.h>
#include <L2DModelAsset.h>
#include <L2DModel.h>
#include <L2DBreath.h>
#include <L2DBlink.h>
#include <L2DPhysics.h>

void RenderModel(const char* modelHome, const char* modelJson,float height)
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
	auto model2 = group.Emplace<Live2D::Model>(&Graphics::Device::MainContext(), &ass, 1024.0f / 768.0f);

	// 添加Live2D对象的呼吸效果
	group.Emplace<Live2D::Breath>(model, Live2D::Breath::Params{});
	group.Emplace<Live2D::Breath>(model2, Live2D::Breath::Params{});

	// 添加Live2D对象的眨眼效果
	group.Emplace<Live2D::Blink>(model, 1.0f);
	group.Emplace<Live2D::Blink>(model2, 1.0f);

	// 添加物理效果
	group.Emplace<Live2D::Physics>(model);

	model->SetTranslate({ 0.5f,-0.15f });
	model2->SetTranslate({ -0.5f,-height });
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
