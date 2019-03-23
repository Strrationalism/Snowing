#include "pch.h"
#include <L2DMotion.h>

TEST(Motion, HaruMotion)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Expression.HaruExpressions", { 800,600 }, true);

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());


	Live2D::Device device;

	Live2D::ModelAsset haru{ "Live2D/Haru/","Haru.model3.json" };

	Scene::Group<> s;

	s.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget(),
		Math::Vec4f{ 0,0,0,0 });

	auto model = s.Emplace<Live2D::Model>(
		&Graphics::Device::MainContext(),
		&haru,
		800.0f / 600.0f);

	auto motion = s.Emplace<Live2D::Motion>(model, 0u, 0u);
	motion->Play();

	Engine::Get().RunObject(s);
}
