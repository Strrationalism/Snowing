#include "pch.h"

void LipSyncTest(const char* home, const char* entryJson,float height)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"LipSync.LipSyncTest", { 800,600 }, true);

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());


	Live2D::Device device;

	Live2D::ModelAsset ass{ home,entryJson };

	Scene::Group<> s;

	s.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget(),
		Math::Vec4f{ 0,0,0,0 });

	auto model = s.Emplace<Live2D::Model>(
		&Graphics::Device::MainContext(),
		&ass,
		800.0f / 600.0f);

	auto lipSync = s.Emplace<Live2D::LipSync>(
		model);

	model->SetTranslate({ 0,-height });
	model->SetScale({ 4.0F,4.0F });

	s.Emplace<Scene::VirtualFrameFunc>([lipSync] {
		lipSync->SetVolume(float(rand()) / RAND_MAX);
		return true;
	});

	s.Emplace<Scene::PointerTask>(5.0f,[] {
		Snowing::Engine::Get().Exit();
	});

	Snowing::Engine::Get().RunObject(s);
}

TEST(LipSync, Haru)
{
	LipSyncTest("Live2D/Haru/", "Haru.model3.json", 2.7f);
}

TEST(LipSync, Hiyori)
{
	LipSyncTest("Live2D/Hiyori/", "Hiyori.model3.json", 1.7f);
}

