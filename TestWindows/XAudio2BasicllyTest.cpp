#include "pch.h"
#include "Platforms.h"
#include <XASoundPlayer.h>

using namespace std::chrono;

static void PlayXAudio(const wchar_t* testName,const char* name,std::chrono::seconds secs,uint32_t begin = 0,float speed = 1.0f)
{
	WindowImpl window{ testName ,WinSize };
	D3D::Device device{ window.GetHWND(),true };
	Engine engine;

	PlatformImpls::WindowsImpl::XAudio2::XADevice audDev;
	if (!audDev.Avaliable()) return;
	assert(&audDev.Get() == &audDev);

	Blob blob = LoadAsset(name);
	Audio::SoundPlayer player;
	player.Play(&blob,begin);
	player.SetSpeed(speed);

	std::chrono::high_resolution_clock clk;
	auto beg = clk.now();
	engine.Run([&clk, beg, secs] {
		if (clk.now() - beg > secs)
			Engine::Get().Exit();
	});
}

TEST(XAudio2Test, PlayBGM)
{
	PlayXAudio(L"TestPlayBGM","Dir/Sound/ExBoss.snd",25s,0,60);
}

TEST(XAudio2Test, PlayBGMBegin)
{
	PlayXAudio(L"TestPlayBGM", "Dir/Sound/ExBoss.snd", 10s,3000000);
}

TEST(XAudio2Test, PlayHeadOnly)
{
	PlayXAudio(L"TestPlayHeadOnly","Dir/Sound/HeadOnly.snd",5s);
}

TEST(XAudio2Test, PlayLoopOnly)
{
	PlayXAudio(L"TestPlayLoopOnly","Dir/Sound/LoopOnly.snd",5s);
}

TEST(XAudio2Test, MultiSound_Breakpoint_Pan)
{
	WindowImpl window{ L"MultiSound" ,WinSize };
	D3D::Device device{ window.GetHWND(),true };
	Engine engine;

	PlatformImpls::WindowsImpl::XAudio2::XADevice audDev;
	assert(&audDev.Get() == &audDev);

	Blob bo1{ LoadAsset("Dir/Sound/ExBoss.snd") }, bo2{ LoadAsset("Dir/Sound/HeadOnly.snd") };
	Audio::SoundPlayer player;
	player.SetVolume(1);
	player.SetPan(1);
	player.Play(&bo1);

	Audio::SoundPlayer player2;
	player2.Play(&bo2);

	std::chrono::high_resolution_clock clk;
	auto beg = clk.now();
	std::uint32_t breakPoint = 0;

	bool b1 = true, b2 = true, b3 = true;
	engine.Run([&] {
		if (clk.now() - beg > 10s)
			Engine::Get().Exit();

		std::cout << "Playing:" << player2.GetPlaying() << std::endl;

		if (clk.now() - beg > 3s && b1)
		{
			b1 = false;
			breakPoint = player.Stop();
		}

		if (clk.now() - beg > 4s && b2)
		{
			b2 = false;
			player.SetPan(-1);
			player.Play(&bo1,breakPoint);
		}

		if (clk.now() - beg > 6s && b3)
		{
			b3 = false;
			player.SetPan(0);
			player.SetVolume(0.05f);
			player.SetSpeed(2.0f);
		}
	});
}