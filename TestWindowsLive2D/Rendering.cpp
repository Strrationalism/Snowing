#include "pch.h"
#include <future>

void RenderModel(const char* modelHome, const char* modelJson,float height)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Rendering.RenderModel", { 1920 / 3 * 2,1080 / 3 * 2 }, true);

	// ����Live2D�豸
	Live2D::Device device;

	// �첽����Live2D��Դ
	auto assFut = std::async([modelHome, modelJson]{
		return std::make_unique<Live2D::ModelAsset>( modelHome,modelJson );
	});
	assFut.wait();
	auto ass = assFut.get();

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());


	// ��Ļ����ϵ
	Math::Coordinate2DRect Coord
	{
		{-512,-384},
		{512,384}
	};

	auto font = LoadFont(LoadAsset("Font-chs.fnt"));
	Effect eff{ LoadAsset("DebugDisplay.cso") };
	auto tech = eff.LoadTechnique("DebugDisplay", Sprite::DataLayout);

	// ������
	Scene::Group<> group;
	group.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget(),
		Math::Vec4f{ 0,0,0,0 });

	group.Emplace<Scene::VirtualTask>(5.0f,[] {Engine::Get().Exit(); });

	// һ��Live2D����
	auto model = group.Emplace<Live2D::Model>(&Graphics::Device::MainContext(),ass.get(), 1920.0f / 1080.0f);
	auto model2 = group.Emplace<Live2D::Model>(&Graphics::Device::MainContext(), ass.get(), 1920.0f / 1080.0f);

	// ���Live2D����ĺ���Ч��
	group.Emplace<Live2D::Breath>(model, Live2D::Breath::Params{});
	group.Emplace<Live2D::Breath>(model2, Live2D::Breath::Params{});

	// ���Live2D�����գ��Ч��
	group.Emplace<Live2D::Blink>(model, 1.0f);
	group.Emplace<Live2D::Blink>(model2, 1.0f);

	// �������Ч��
	group.Emplace<Live2D::Physics>(model);

	model->SetTranslate({ 256.0f,-0.15f*384.0f }, Coord);
	model2->SetTranslate({ -256.0f,-height * 384.0f }, Coord);
	model2->SetScale({ 4,4 });


	group.Emplace<Scene::Debug::DebugDisplay>(
		&tech, &font, L"FPS", Scene::Debug::DebugDisplay::FPSGetter);

	group.Emplace<Scene::Debug::DebugDisplay>(
		&tech, &font, L"Frame Time", Scene::Debug::DebugDisplay::FrameTimeGetter);

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
