#include "pch.h"
#include "AudioChannel.h"
#include "SEPlayer.h"
#include "CVPlayer.h"
using namespace Snowing;
using namespace Yukimi;

constexpr auto AudioLoader = [] (auto s) { return Snowing::LoadAsset(s); };

TEST(Audio, AudioChannel)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Audio.AudioChannel", { 400,300 }, true);

	PlatformImpls::WindowsImpl::XAudio2::XADevice x;

	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("FontRendering.cso") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("Font", Sprite::DataLayout);

	Scene::Group<> scene;
	scene.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),&Graphics::Device::MainRenderTarget());
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"Time", Scene::Debug::DebugDisplay::FrameTimeGetter);
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"FPS", Scene::Debug::DebugDisplay::FPSGetter);
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"Scene Objs", [&scene] { return std::to_wstring(scene.Count()); });

	scene.Emplace<Scene::VirtualTask>(0.5f,[&] {
		scene.Emplace<AudioChannel>(AudioLoader,"Dir/Sound/ExBoss.snd", 0.5f, 90000u);
		scene.Emplace<Scene::VirtualTask>(0.5f, [&] {
			scene.IterType<AudioChannel>([](auto & player) {
				player.Pause(0.5f);
			});
		});

		scene.Emplace<Scene::VirtualTask>(1.0f, [&] {
			scene.IterType<AudioChannel>([](auto & player) {
				player.Resume(0.5f);
			});
		});

		scene.Emplace<Scene::VirtualTask>(2.0f, [&] {
			scene.IterType<AudioChannel>([](auto & player) {
				player.Stop(0.5f);
			});
		});
	});


	scene.Emplace<Scene::VirtualTask>(3.5f, [&] {
		Engine::Get().Exit();
	});

	Engine::Get().RunObject(scene);
}

TEST(Audio, SEPlayer)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Audio.SEPlayer", { 400,300 }, true);

	PlatformImpls::WindowsImpl::XAudio2::XADevice x;

	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("FontRendering.cso") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("Font", Sprite::DataLayout);

	Scene::Group<> scene;
	scene.Emplace<SEPlayer>(AudioLoader);
	scene.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(), &Graphics::Device::MainRenderTarget());
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"Time", Scene::Debug::DebugDisplay::FrameTimeGetter);
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"FPS", Scene::Debug::DebugDisplay::FPSGetter);
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"Scene Objs", [&scene] { return std::to_wstring(scene.Count()); });

	scene.Emplace<Scene::VirtualTask>(0.5f, [&] {
		SEPlayer::Get().Play("Dir/Sound/HeadOnly.snd");

		scene.Emplace<Scene::VirtualTask>(0.5f, [&] {
			SEPlayer::Get().Play("Dir/Sound/HeadOnly.snd",1);
		});

		scene.Emplace<Scene::VirtualTask>(1.0f, [&] {
			SEPlayer::Get().Play("Dir/Sound/HeadOnly.snd", -1);
		});

		scene.Emplace<Scene::VirtualTask>(1.5f, [&] {
			SEPlayer::Get().Play("Dir/Sound/HeadOnly.snd", 1,0.5F);
		});

		scene.Emplace<Scene::VirtualTask>(2.0f, [&] {
			SEPlayer::Get().Play("Dir/Sound/HeadOnly.snd", -1,0.5F);
		});
	});


	scene.Emplace<Scene::VirtualTask>(2.5f, [&] {
		Engine::Get().Exit();
	});

	Engine::Get().RunObject(scene);
}

TEST(Audio, CVPlayer)
{
	auto engine =
		PlatformImpls::WindowsImpl::MakeEngine(L"Audio.CVPlayer", { 400,300 }, true);

	PlatformImpls::WindowsImpl::XAudio2::XADevice x;

	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("FontRendering.cso") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("Font", Sprite::DataLayout);
	
	Scene::Group<> scene;
	auto cv = scene.Emplace<CVPlayer>(AudioLoader);
	scene.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(), &Graphics::Device::MainRenderTarget());
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"Time", Scene::Debug::DebugDisplay::FrameTimeGetter);
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"FPS", Scene::Debug::DebugDisplay::FPSGetter);
	scene.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"Scene Objs", [&scene] { return std::to_wstring(scene.Count()); });

	scene.Emplace<Scene::VirtualTask>(0.5f, [&, cv] {
		cv->Play("Dir/Sound/HeadOnly.snd");
	});

	scene.Emplace<Scene::VirtualTask>(0.75f, [&, cv] {
		auto& player = *cv;
		player.Play("Dir/Sound/HeadOnly.snd");
		player.VolumeDown();
	});

	scene.Emplace<Scene::VirtualTask>(1.0f, [&, cv] {
		auto& player = *cv;
		player.Play("Dir/Sound/HeadOnly.snd");
		
	});

	scene.Emplace<Scene::VirtualTask>(2.5f, [&] {
		Engine::Get().Exit();
	});

	Engine::Get().RunObject(scene);
}
