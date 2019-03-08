#include "pch.h"
#include "Task.h"
#include "Tween.h"
#include "Group.h"
#include <XInputController.h>

void RunGameObject(Snowing::Scene::Object& obj,const wchar_t* testName)
{
	WindowImpl window{ testName ,WinSize };
	D3D::Device device{ window.GetHWND(),true };
	Engine engine;
	engine.RunObject(obj);
}

TEST(Scene, EmptyObject)
{
	class EmptyObject : public Snowing::Scene::Object
	{
	private:
		float t = 0.25f;
	public:
		bool Update() override
		{
			t -= Engine::Get().DeltaTime();
			return t > 0;
		}
	};

	RunGameObject(EmptyObject{}, L"EmptyObject");
}

TEST(Scene, TaskObject)
{
	Scene::Task t{ 0.2f, []() {Engine::Get().Exit(); } };
	RunGameObject(t,L"TaskObject");
}

TEST(Scene, VirtualTaskObject)
{
	Scene::VirtualTask t{ 0.1f,[]() {Engine::Get().Exit(); } };
	RunGameObject(t, L"VirtualTaskObject");
}

class TestTween final : public Scene::Object
{
private:
	Scene::Tween<Math::Vec2f> col_;
public:
	TestTween()
	{
		col_.Start({ 1.0f,0.0f }, 0.25f, Scene::TweenFX::Once);
	}

	bool Update() override
	{
		const bool go = col_.Update();
		Engine::Get().Draw([this] {
			auto& d = Device::Get();
			d.MainContext().ClearRenderTarget(d.MainRenderTarget(), { col_->x,col_->y,0.0f,1.0f });
		});

		return go;
	}
};

TEST(Scene, Tween)
{
	TestTween t;
	RunGameObject(t, L"Tween");
}

TEST(Scene, Group)
{
	Scene::Group p;
	p.Emplace<Scene::VirtualTask>(0.5f, []() {Engine::Get().Exit(); });
	p.Emplace<TestTween>();
	RunGameObject(p, L"Group");
}

TEST(Scene, TweenSetter)
{
	
	class MyObject final : public Scene::Group<Scene::VirtualTweenSetter<float>>
	{
	private:
		float t_ = 0;
	public:

		MyObject()
		{
			auto setter = [this](float a)
			{
				t_ = a;
			};

			auto p = Emplace<>(0.0f, setter);
			p->Start(1.0f, 0.25f);
			*p = 0.5f;
			p->Start(0.0f, 0.5f,Scene::TweenFX::Sin);
		}

		bool Update() override
		{
			Engine::Get().Draw([this] {
				auto& d = Device::Get();
				d.MainContext().ClearRenderTarget(d.MainRenderTarget(), { t_,t_,t_,1.0f });
			});
			return Scene::Group<Scene::VirtualTweenSetter<float>>::Update();
		}
	};

	RunGameObject(MyObject{},L"TweenSetter");
}


TEST(Scene, VibratePlayer)
{
	Scene::VibrateEffect effect{LoadAsset("VibratorEffect.csv")};

	auto controllers = Snowing::PlatformImpls::WindowsXInput::GetConnectedControllers();
	if (!controllers.empty())
	{
		auto controller = std::move(controllers.front());
		Scene::Group<> g;
		g.Emplace<Scene::VibratePlayer<PlatformImpls::WindowsXInput::XInputController>>(&controller,&effect);
		g.Emplace<Scene::VirtualTask>(5.0f, [] {Engine::Get().Exit(); });
		RunGameObject(g, L"PlayVibrateEffect");
	}
	
}

TEST(Scene, Menu)
{
	class DemoMenu : public Scene::Object
	{
	private:
		size_t i_;
	public:
		DemoMenu(size_t i) :i_{ i } {}

		void OnSelected()
		{
			Snowing::Log("Menu Selected:", i_);
		}

		void OnUnselected()
		{
			Snowing::Log("Menu Unselected:", i_);
		}
	};

	Scene::UI::Menu<DemoMenu> menu;

	for(size_t i = 0;i < 5;++i)
		menu.Emplace(i);
	menu.Update();

	Assert(menu.GetSelectedObject(), std::nullopt);
	Assert(menu.GetSelectedIndex(), std::nullopt);

	menu.Select(3);
	Assert(menu.GetSelectedIndex(), 3);

	menu.Select(4);
	Assert(menu.GetSelectedIndex(), 4);

	menu.Select(0);
	Assert(menu.GetSelectedIndex(), 0);

	menu.Unselect();
	Assert(menu.GetSelectedObject(), std::nullopt);
	Assert(menu.GetSelectedIndex(), std::nullopt);
}

TEST(Scene, TextMenuItem)
{
	using namespace std;
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"TextMenuItem",
		{ 800,600 },
		true);

	class MyScene : public Scene::Group<>
	{
	private:
		Math::Coordinate2DRect coord
		{
			{ -(float)WinSize.x / 2,-(float)WinSize.y / 2 },
			{ (float)WinSize.x / 2, (float)WinSize.y / 2 } };

		Snowing::Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
		Snowing::Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestLighting", Sprite::DataLayout);
		Snowing::Graphics::EffectTech tech2 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
		Snowing::Graphics::Buffer gpuVBuffer = FontRenderer<>::MakeGPUVertexBuffer();

		Font font = LoadFont(LoadAsset("Font-chs.fnt"));
		FontRenderer<> fontRenderer
		{
			&Graphics::Device::Get().MainContext(),
			&tech1,
			coord,
			&font,
			&gpuVBuffer
		};

		Scene::UI::Menu<Scene::UI::TextMenuItem> menu_;
	public:
		MyScene()
		{
			constexpr Math::Vec4f box1{ -400.0f,-200.0f,800.0f,64.0f };
			constexpr Math::Vec4f box2{ -400.0f,-150.0f,800.0f,64.0f };
			constexpr Math::Vec4f box3{ -400.0f,-100.0f,800.0f,64.0f };
			constexpr Math::Vec4f box4{ -400.0f,-50.0f,800.0f,64.0f };
			constexpr Math::Vec2f space{ 1.0f,1.0f };
			constexpr Math::Vec2f fontSize{ 0.5f,0.5f };

			menu_.Emplace(&fontRenderer, L"Strrationalism"sv, box1, space, fontSize);
			menu_.Emplace(&fontRenderer, L"test"sv, box2, space, fontSize);
			menu_.Emplace(&fontRenderer, L"text"sv, box3, space, fontSize);
			menu_.Emplace(&fontRenderer, L"menu"sv, box4, space, fontSize);

			Emplace<Scene::VirtualTask>(0.1f, [&] {menu_.Select(0); });
			Emplace<Scene::VirtualTask>(0.2f, [&] {menu_.Select(1); });
			Emplace<Scene::VirtualTask>(0.3f, [&] {menu_.Select(2); });
			Emplace<Scene::VirtualTask>(0.4f, [&] {menu_.Select(3); });
			Emplace<Scene::VirtualTask>(0.5f, [&] {menu_.Unselect(); });
			Emplace<Scene::VirtualTask>(0.6f, [&] {menu_.Select(1); });
			Emplace<Scene::VirtualTask>(0.7f, [&] {Engine::Get().Exit(); });
		}

		bool Update() override
		{
			Scene::Group<>::Update();

			Engine::Get().Draw([] {
				Graphics::Device::Get().MainContext().SetRenderTarget(
					&Graphics::Device::Get().MainRenderTarget());
			});

			menu_.Update();
			return true;// Scene::Group<>::Update();
		}
	};

	Snowing::Engine::Get().RunObject(MyScene{});
}


TEST(Scene, MenuKeyController)
{
	using namespace std;
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"TextMenuItem",
		{ 800,600 },
		true);

	class MyScene : public Scene::Group<>
	{
	private:
		Math::Coordinate2DRect coord
		{
			{ -(float)WinSize.x / 2,-(float)WinSize.y / 2 },
			{ (float)WinSize.x / 2, (float)WinSize.y / 2 } };

		Snowing::Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
		Snowing::Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestLighting", Sprite::DataLayout);
		Snowing::Graphics::EffectTech tech2 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
		Snowing::Graphics::Buffer gpuVBuffer = FontRenderer<>::MakeGPUVertexBuffer();

		Font font = LoadFont(LoadAsset("Font-chs.fnt"));
		FontRenderer<> fontRenderer
		{
			&Graphics::Device::Get().MainContext(),
			&tech1,
			coord,
			&font,
			&gpuVBuffer
		};

		Scene::UI::Menu<Scene::UI::TextMenuItem> menu_;
		Scene::UI::MenuKeyController<Scene::UI::TextMenuItem> menuCtrl_{ &menu_ };

	public:
		MyScene()
		{
			constexpr Math::Vec4f box1{ -400.0f,-200.0f,800.0f,64.0f };
			constexpr Math::Vec4f box2{ -400.0f,-150.0f,800.0f,64.0f };
			constexpr Math::Vec4f box3{ -400.0f,-100.0f,800.0f,64.0f };
			constexpr Math::Vec4f box4{ -400.0f,-50.0f,800.0f,64.0f };
			constexpr Math::Vec2f space{ 1.0f,1.0f };
			constexpr Math::Vec2f fontSize{ 0.5f,0.5f };
			std::map<wchar_t, Math::Vec2f> f{};

			menu_.Emplace(&fontRenderer, L"Strrationalism"sv, box1, space, fontSize);
			menu_.Emplace(&fontRenderer, L"test"sv, box2, space, fontSize);
			menu_.Emplace(&fontRenderer, L"text"sv, box3, space, fontSize);
			menu_.Emplace(&fontRenderer, L"menu"sv, box4, space, fontSize);

			for(int i = 0;i < 7;++ i)
				Emplace<Scene::VirtualTask>(0.05f + 0.05f * i, [this] {menuCtrl_.Next(); });

			for (int i = 0; i < 7; ++i)
				Emplace<Scene::VirtualTask>(0.05f + 0.05f * (i + 8), [this] {menuCtrl_.Prev(); });

			Emplace<Scene::VirtualTask>(0.05f * 16, [&] {Engine::Get().Exit(); });
		}

		bool Update() override
		{
			Scene::Group<>::Update();

			Engine::Get().Draw([] {
				Graphics::Device::Get().MainContext().SetRenderTarget(
					&Graphics::Device::Get().MainRenderTarget());
			});

			menu_.Update();
			return true;
		}
	};

	Snowing::Engine::Get().RunObject(MyScene{});
}

TEST(Scene,MenuPositionController)
{
	using namespace std;
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"TextMenuItem",
		{ 800,600 },
		true);

	class MyScene : public Scene::Group<>
	{
	private:
		Math::Coordinate2DRect coord
		{
			{ -(float)WinSize.x / 2,-(float)WinSize.y / 2 },
			{ (float)WinSize.x / 2, (float)WinSize.y / 2 } };

		Snowing::Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
		Snowing::Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestLighting", Sprite::DataLayout);
		Snowing::Graphics::EffectTech tech2 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
		Snowing::Graphics::Buffer gpuVBuffer = FontRenderer<>::MakeGPUVertexBuffer();

		Font font = LoadFont(LoadAsset("Font-chs.fnt"));
		FontRenderer<> fontRenderer
		{
			&Graphics::Device::Get().MainContext(),
			&tech1,
			coord,
			&font,
			&gpuVBuffer
		};

		Scene::UI::Menu<Scene::UI::TextMenuItem> menu_;
		Scene::UI::MenuPositionController<
			Scene::UI::TextMenuItem,
			Input::Input,
			Input::MousePosition>
		menuCtrl_{
			&menu_,coord,
			&Input::Input::Get(),Input::MousePosition::CoordinateSystem,Input::MousePosition{} };

	public:
		MyScene()
		{
			constexpr Math::Vec4f box1{ -400.0f,-200.0f,800.0f,64.0f };
			constexpr Math::Vec4f box2{ -400.0f,-150.0f,800.0f,64.0f };
			constexpr Math::Vec4f box3{ -400.0f,-100.0f,800.0f,64.0f };
			constexpr Math::Vec4f box4{ -400.0f,-50.0f,800.0f,64.0f };
			constexpr Math::Vec2f space{ 1.0f,1.0f };
			constexpr Math::Vec2f fontSize{ 0.5f,0.5f };

			menu_.Emplace(&fontRenderer, L"Strrationalism"sv, box1, space, fontSize);
			menu_.Emplace(&fontRenderer, L"test"sv, box2, space, fontSize);
			menu_.Emplace(&fontRenderer, L"text"sv, box3, space, fontSize);
			menu_.Emplace(&fontRenderer, L"menu"sv, box4, space, fontSize);

			Emplace<Scene::VirtualTask>(1.0f, [&] {Engine::Get().Exit(); });
		}

		bool Update() override
		{
			Scene::Group<>::Update();

			Engine::Get().Draw([] {
				Graphics::Device::Get().MainContext().SetRenderTarget(
					&Graphics::Device::Get().MainRenderTarget());
			});

			menu_.Update();
			menuCtrl_.Update();
			return true;
		}
	};

	Snowing::Engine::Get().RunObject(MyScene{});
}

static void TestDebugDisplay(Math::Vec2<int> size)
{
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"FPSDisplay",
		size,
		true);
	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
	Scene::Group<> g;
	g.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget());

	g.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font,L"Time", Scene::Debug::DebugDisplay::FrameTimeGetter);
	g.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"FPS", Scene::Debug::DebugDisplay::FPSGetter);
	g.Emplace<Scene::VirtualTask>(0.25f, [] {Engine::Get().Exit(); });

	Engine::Get().RunObject(g);
}

TEST(Scene, DebugDisplay)
{
	TestDebugDisplay({ 800,600 });
	TestDebugDisplay({ 400,300 });
	TestDebugDisplay({ 1280,720 });
	TestDebugDisplay({ 960,300 });
}

TEST(Scene, DebugMenu)
{
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"DebugMenu",
		{ 800,600 },
		true);
	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestLighting", Sprite::DataLayout);
	Scene::Group<> g;
	g.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget());

	auto debugMenu = g.Emplace<Scene::Debug::DebugMenu>(&tech1, &font);
	debugMenu->AddMenuItem(L"Write Logger", []
	{
		Snowing::Log("debugMenu write log");
	});
	debugMenu->AddMenuItem(L"Write Logger2", []
	{
		Snowing::Log("debugMenu write log2");
	});

	g.Emplace<Scene::VirtualTask>(1.0f, [] {Engine::Get().Exit(); });

	Engine::Get().RunObject(g);
}

TEST(Scene, ConditionTask)
{
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"ConditionTask",
		{ 800,600 },
		true);

	int i = 0;

	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
	Scene::Group<> g;
	g.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget());

	Scene::ConditionTask task
	{
		[&i] { return i >= 60; },
		[] {Engine::Get().Exit(); }
	};

	g.Emplace<decltype(task)>(std::move(task));
	g.Emplace<Scene::Debug::DebugDisplay>(
		&tech1, &font, L"i", [&i] { return std::to_wstring(i); });

	Engine::Get().Run(
		[&] {
		i++;
		g.Update();
	});
}
