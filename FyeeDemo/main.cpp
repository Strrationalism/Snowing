#define USE_XAUDIO2
#include <Snowing.h>
#include <Fyee.h>
#include <memory.h>
using namespace Snowing;
using namespace Fyee;

const auto HeadBlob = std::make_shared<Snowing::Blob>(Snowing::LoadAsset("Fyee/Head.snd"));
const auto Loop1Blob = std::make_shared<Snowing::Blob>(Snowing::LoadAsset("Fyee/Loop1.snd"));
const auto Melody1Blob = std::make_shared<Snowing::Blob>(Snowing::LoadAsset("Fyee/Melody1.snd"));
const auto Melody2Blob = std::make_shared<Snowing::Blob>(Snowing::LoadAsset("Fyee/Melody2.snd"));
const auto EndingBlob = std::make_shared<Snowing::Blob>(Snowing::LoadAsset("Fyee/Ending.snd"));

const TrackInfo Head
{
	HeadBlob,
	{0,2},
	false,
	[] { Snowing::Log("Head"); }
};

const TrackInfo Loop1
{
	Loop1Blob,
	{2,0},
	true,
	[] { Snowing::Log("Loop1"); }
};

const TrackInfo Melody1
{
	Melody1Blob,
	{ 8,0 },
		true,
		[&] {
		Snowing::Log("Melody1");
	}
};

const TrackInfo Melody2
{
	Melody2Blob,
	{ 8,0 },
		true,
		[&] {
		Snowing::Log("Melody2");
	}
};

const TrackInfo Ending
{
	EndingBlob,
	{ 1,0 },
	false,
	[&] {
		Snowing::Log("Ending");
	}
};



void NextMenu(Scene::Group<>& scene, const Graphics::Font& font, Graphics::EffectTech& tech)
{
	auto menu = scene.Emplace<Scene::Debug::DebugMenu>(&tech, &font);
	menu->AddMenuItem(L"切换到版本A", [&scene]
	{
		scene.IterType<Fyee::BGMPlayer>([](Fyee::BGMPlayer& player)
		{
			player.EditionFading(Melody1);
		});
	});

	menu->AddMenuItem(L"切换到版本B", [&scene]
	{
		scene.IterType<Fyee::BGMPlayer>([](Fyee::BGMPlayer& player)
		{
			player.EditionFading(Melody2);
		});
	});

	menu->AddMenuItem(L"跳转到结尾", [menu,&scene]
	{
		menu->Kill();

		scene.IterType<Fyee::BGMPlayer>([](Fyee::BGMPlayer& player)
		{
			player.ClearQueueTail();
			player.AddToPlayQueue(Ending);
			player.ScheduleBreakLoop(Fyee::BGMPlayer::BreakWhenJumpTime{ BGMPlayer::BreakTime::NextBar,0.5f });
		});

	});
}

TrackInfo GetMelody1(Scene::Group<>& scene, const Graphics::Font& font, Graphics::EffectTech& tech)
{
	return
	{
		Melody1Blob,
		{ 8,0 },
		true,
		[&] { 
			Snowing::Log("Melody1");
			NextMenu(scene,font,tech); 
		}
	};
}




int main()
{
	auto engine =
		Snowing::PlatformImpls::WindowsImpl::MakeEngine(
			L"Fyee Demo",	
			{ 400,300 },	
			true);
	PlatformImpls::WindowsImpl::XAudio2::XADevice xaDevice;

	const Graphics::Font fnt =
		Graphics::LoadFont(
			LoadAsset("Font-chs.fnt"));

	Graphics::Effect effect
	{
		LoadAsset("FontShader.cso")
	};

	auto fontTech = effect.LoadTechnique(
		"FontRendering",
		Graphics::Sprite::DataLayout);

	Scene::Group<> scene;
	scene.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),&Graphics::Device::MainRenderTarget());

	Fyee::BGMPlayer* fyee = nullptr;

	auto startMenu = scene.Emplace<Scene::Debug::DebugMenu>(&fontTech, &fnt);
	startMenu->AddMenuItem(L"开始播放BGM", [&]
	{
		startMenu->Kill();

		fyee = scene.Emplace<Fyee::BGMPlayer>();
		fyee->AddToPlayQueue(Head);
		fyee->AddToPlayQueue(Loop1);

		scene.Emplace<Scene::Debug::DebugDisplay>(
			&fontTech,	
			&fnt,		
			L"节拍器",
			[&scene]
			{
				const auto p = scene.FindFirst<Fyee::BGMPlayer>();
				if (p)
				{
					const auto time = p->GetTime();
					return std::to_wstring(time.Bar) + L":" + std::to_wstring(time.Beat);
				}
				else return std::wstring(L"");
			}
		);



		auto firstMenu = scene.Emplace<Scene::Debug::DebugMenu>(&fontTech, &fnt);

		firstMenu->AddMenuItem(L"现在切换", [firstMenu,&scene,&fnt,&fontTech,fyee]
		{
			firstMenu->Kill();

			fyee->ClearQueueTail();
			fyee->AddToPlayQueue(GetMelody1(scene, fnt, fontTech));
			fyee->ScheduleBreakLoop(Fyee::BGMPlayer::BreakWhenJumpTime{ BGMPlayer::BreakTime::Now,0.5f });
		});

		firstMenu->AddMenuItem(L"从下个Beat开始切换", [firstMenu, &scene, &fnt, &fontTech,fyee]
		{
			firstMenu->Kill();

			fyee->ClearQueueTail();
			fyee->AddToPlayQueue(GetMelody1(scene, fnt, fontTech));
			fyee->ScheduleBreakLoop(Fyee::BGMPlayer::BreakWhenJumpTime{ BGMPlayer::BreakTime::NextBeat,0.5f });
		});

		firstMenu->AddMenuItem(L"从下一Bar开始切换", [firstMenu, &scene, &fnt, &fontTech,fyee]
		{
			firstMenu->Kill();
			fyee->ClearQueueTail();
			fyee->AddToPlayQueue(GetMelody1(scene, fnt, fontTech));
			fyee->ScheduleBreakLoop(Fyee::BGMPlayer::BreakWhenJumpTime{ BGMPlayer::BreakTime::NextBar,0.5f});
		});

		firstMenu->AddMenuItem(L"从下一次循环开始切换", [firstMenu, &scene, &fnt, &fontTech,fyee]
		{
			firstMenu->Kill();
			fyee->ClearQueueTail();
			fyee->AddToPlayQueue(GetMelody1(scene, fnt, fontTech));
			fyee->ScheduleBreakLoop(Fyee::BGMPlayer::BreakOnNextLoop{});
			
		});
	});


	

	// 运行一个对象
	Snowing::Engine::Get().RunObject(scene);

	return 0;
}