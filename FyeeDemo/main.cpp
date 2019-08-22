#include <Snowing.h>
#include <Task.h>

using namespace Snowing;

const auto Head = Snowing::LoadAsset("Fyee/Head.snd");
const auto Loop1 = Snowing::LoadAsset("Fyee/Loop1.snd");
const auto Melody1 = Snowing::LoadAsset("Fyee/Melody1.snd");
const auto Melody2 = Snowing::LoadAsset("Fyee/Melody2.snd");
const auto Ending = Snowing::LoadAsset("Fyee/Ending.snd");

void NextMenu(Scene::Group<>& scene, const Graphics::Font& font,Graphics::EffectTech& tech)
{
	auto menu = scene.Emplace<Scene::Debug::DebugMenu>(&tech ,&font);
	menu->AddMenuItem(L"切换到版本A", []
	{
	});

	menu->AddMenuItem(L"切换到版本B", [] {});
	menu->AddMenuItem(L"跳转到结尾", [menu]
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
	auto firstMenu = scene.Emplace<Scene::Debug::DebugMenu>(&fontTech, &fnt);

	firstMenu->AddMenuItem(L"现在切换", [&]
	{
		firstMenu->Kill();

		NextMenu(scene, fnt, fontTech);
	});

	firstMenu->AddMenuItem(L"从下个Step开始切换", [&]
	{
		firstMenu->Kill();

		NextMenu(scene, fnt, fontTech);
	});

	firstMenu->AddMenuItem(L"从下一Beat开始切换", [&]
	{
		firstMenu->Kill();

		NextMenu(scene, fnt, fontTech);
	});
	

	// 运行一个对象
	Snowing::Engine::Get().RunObject(scene);

	return 0;
}