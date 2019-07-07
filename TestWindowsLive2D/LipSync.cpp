#include "pch.h"

void LipSyncTest(const char* home, const char* entryJson,float height)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"LipSync.LipSyncTest", { 800,600 }, true);

	PlatformImpls::WindowsImpl::XAudio2::XADevice d;

	Graphics::Device::MainContext().SetRenderTarget(
		&Graphics::Device::MainRenderTarget());

	auto font = LoadFont(LoadAsset("Font-chs.fnt"));
	Effect eff{ LoadAsset("DebugDisplay.cso") };
	auto tech = eff.LoadTechnique("DebugDisplay", Sprite::DataLayout);

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

	auto sound = LoadAsset("Dir/Sound/HeadOnly.snd");

	Audio::SoundPlayer soundPlayer;

	s.Emplace<Scene::VirtualTask>(2.0f, [&] {
		soundPlayer.Play(&sound);
	});

	s.Emplace<Scene::VirtualFrameFunc>([lipSync,&soundPlayer] {
		const float t = soundPlayer.GetRealtimeVolume();
		Snowing::Log(t);
		lipSync->SetVolume(t * 2);
		return true;
	});

	s.Emplace<Scene::PointerTask>(5.0f,[] {
		Snowing::Engine::Get().Exit();
	});

	

	s.Emplace<Scene::Debug::DebugDisplay>(
		&tech, &font, L"RealtimeVolume", [&] {
		return std::to_wstring(soundPlayer.GetRealtimeVolume());
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

