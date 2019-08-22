#define USE_XAUDIO2
#include <Snowing.h>
#include <Fyee.h>
using namespace Snowing;
using namespace Fyee;

const auto HeadBlob = Snowing::LoadAsset("Fyee/Head.snd");
const auto Loop1Blob = Snowing::LoadAsset("Fyee/Loop1.snd");
const auto Melody1Blob = Snowing::LoadAsset("Fyee/Melody1.snd");
const auto Melody2Blob = Snowing::LoadAsset("Fyee/Melody2.snd");
const auto EndingBlob = Snowing::LoadAsset("Fyee/Ending.snd");

const TrackInfo Head
{
	&HeadBlob,
	{0,2},
	false,
	[] { Snowing::Log("Head"); }
};

const TrackInfo Loop1
{
	&Loop1Blob,
	{2,0},
	true,
	[] { Snowing::Log("Loop1"); }
};

void NextMenu(Scene::Group<>& scene, const Graphics::Font& font,Graphics::EffectTech& tech)
{
	auto menu = scene.Emplace<Scene::Debug::DebugMenu>(&tech ,&font);
	menu->AddMenuItem(L"�л����汾A", []
	{
	});

	menu->AddMenuItem(L"�л����汾B", [] {});
	menu->AddMenuItem(L"��ת����β", [menu]
	{
		menu->Kill();
	});
}

int main()
{
	auto engine =
		Snowing::PlatformImpls::WindowsImpl::MakeEngine(
			L"HelloWorld",	
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
	startMenu->AddMenuItem(L"��ʼ����BGM", [&]
	{
		startMenu->Kill();

		fyee = scene.Emplace<Fyee::BGMPlayer>();
		fyee->AddToPlayQueue(Head);
		fyee->AddToPlayQueue(Loop1);


		auto firstMenu = scene.Emplace<Scene::Debug::DebugMenu>(&fontTech, &fnt);

		firstMenu->AddMenuItem(L"�����л�", [firstMenu,&scene,&fnt,&fontTech]
		{
			firstMenu->Kill();

			NextMenu(scene, fnt, fontTech);
		});

		firstMenu->AddMenuItem(L"���¸�Step��ʼ�л�", [firstMenu, &scene, &fnt, &fontTech]
		{
			firstMenu->Kill();

			NextMenu(scene, fnt, fontTech);
		});

		firstMenu->AddMenuItem(L"����һBeat��ʼ�л�", [firstMenu, &scene, &fnt, &fontTech]
		{
			firstMenu->Kill();

			NextMenu(scene, fnt, fontTech);
		});
	});


	

	// ����һ������
	Snowing::Engine::Get().RunObject(scene);

	return 0;
}