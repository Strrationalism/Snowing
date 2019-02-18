#include "pch.h"
#include "Task.h"
#include "Tween.h"
#include "Group.h"

void RunGameObject(Snowing::Scene::Object& obj,const wchar_t* testName)
{
	WindowImpl window{ testName ,WinSize };
	D3D::Device device{ window.GetHWND(),true };
	Engine engine;
	engine.RunObject(obj);
}

TEST(SceneTest, TestEmptyObject)
{
	class EmptyObject : public Snowing::Scene::Object
	{
	private:
		float t = 1;
	public:
		bool Update() override
		{
			t -= Engine::Get().DeltaTime();
			return t > 0;
		}
	};

	RunGameObject(EmptyObject{}, L"TestEmptyObject");
}

TEST(SceneTest, TestTaskObject)
{
	Scene::Task t{ 1.0f, []() {Engine::Get().Exit(); } };
	RunGameObject(t,L"TestTaskObject");
}

TEST(SceneTest, TestVirtualTaskObject)
{
	Scene::VirtualTask t{ 1,[]() {Engine::Get().Exit(); } };
	RunGameObject(t, L"TestVirtualTaskObject");
}

class TestTween final : public Scene::Object
{
private:
	Scene::Tween<Math::Vec2f> col_;
public:
	TestTween()
	{
		col_.Start({ 1.0f,0.0f }, 1, Scene::TweenFX::Once);
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

TEST(SceneTest, TestTween)
{
	TestTween t;
	RunGameObject(t, L"TestTween");
}

TEST(SceneTest, TestGroup)
{
	Scene::Group p;
	p.Emplace<Scene::VirtualTask>(2.0f, []() {Engine::Get().Exit(); });
	p.Emplace<TestTween>();
	RunGameObject(p, L"TestGroup");
}

TEST(SceneTest, TestTweenSetter)
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

			auto& p = Emplace<>(0.0f, setter);
			p.Start(1.0f, 1);
			p = 0.5f;
			p.Start(0.0f, 1,Scene::TweenFX::Sin);
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

	RunGameObject(MyObject{},L"TestTweenSetter");
}


TEST(SceneTest, TestVibratePlayer)
{
	Scene::VibrateEffect effect{LoadAsset("VibratorEffect.csv")};

	auto controllers = Snowing::PlatformImpls::WindowsXInput::GetConnectedControllers();
	if (!controllers.empty())
	{
		auto controller = std::move(controllers.front());
		Scene::Group<> g;
		g.Emplace<Scene::VibratePlayer<PlatformImpls::WindowsXInput::XInputController>>(&controller,&effect);
		g.Emplace<Scene::VirtualTask>(5.0f, [] {Engine::Get().Exit(); });
		RunGameObject(g, L"TestPlayVibrateEffect");
	}
	
}

TEST(SceneTest, TestMenu)
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

	Scene::Menu<DemoMenu> menu;

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

TEST(SceneTest, TestTextMenuItem)
{
	using namespace std;
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"TestTextMenuItem",
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

		Font font = LoadFont(LoadAsset(L"Font-zh-CN.fnt"));
		FontRenderer<> fontRenderer
		{
			&Graphics::Device::Get().MainContext(),
			&eff,
			&tech1,
			coord,
			&font,
			&gpuVBuffer
		};

		Scene::Menu<Scene::TextMenuItem> menu_;
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

			menu_.Emplace(&fontRenderer, L"Strrationalism"sv, box1, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"test"sv, box2, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"text"sv, box3, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"menu"sv, box4, space, fontSize, f);

			Emplace<Scene::VirtualTask>(0.25f, [&] {menu_.Select(0); });
			Emplace<Scene::VirtualTask>(0.5f, [&] {menu_.Select(1); });
			Emplace<Scene::VirtualTask>(0.75f, [&] {menu_.Select(2); });
			Emplace<Scene::VirtualTask>(1.0f, [&] {menu_.Select(3); });
			Emplace<Scene::VirtualTask>(1.25f, [&] {menu_.Unselect(); });
			Emplace<Scene::VirtualTask>(1.5f, [&] {menu_.Select(1); });
			Emplace<Scene::VirtualTask>(1.75f, [&] {Engine::Get().Exit(); });
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


TEST(SceneTest, TestMenuKeyController)
{
	using namespace std;
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"TestTextMenuItem",
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

		Font font = LoadFont(LoadAsset(L"Font-zh-CN.fnt"));
		FontRenderer<> fontRenderer
		{
			&Graphics::Device::Get().MainContext(),
			&eff,
			&tech1,
			coord,
			&font,
			&gpuVBuffer
		};

		Scene::Menu<Scene::TextMenuItem> menu_;
		Scene::MenuKeyController<Scene::TextMenuItem> menuCtrl_{ &menu_ };

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

			menu_.Emplace(&fontRenderer, L"Strrationalism"sv, box1, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"test"sv, box2, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"text"sv, box3, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"menu"sv, box4, space, fontSize, f);

			for(int i = 0;i < 7;++ i)
				Emplace<Scene::VirtualTask>(0.25f + 0.25f * i, [this] {menuCtrl_.Next(); });

			for (int i = 0; i < 7; ++i)
				Emplace<Scene::VirtualTask>(0.25f + 0.25f * (i + 8), [this] {menuCtrl_.Prev(); });

			Emplace<Scene::VirtualTask>(0.25f * 16, [&] {Engine::Get().Exit(); });
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

TEST(SceneTest, TestMenuPositionController)
{
	using namespace std;
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"TestTextMenuItem",
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

		Font font = LoadFont(LoadAsset(L"Font-zh-CN.fnt"));
		FontRenderer<> fontRenderer
		{
			&Graphics::Device::Get().MainContext(),
			&eff,
			&tech1,
			coord,
			&font,
			&gpuVBuffer
		};

		Scene::Menu<Scene::TextMenuItem> menu_;
		Scene::MenuPositionController<
			Scene::TextMenuItem,
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
			std::map<wchar_t, Math::Vec2f> f{};

			menu_.Emplace(&fontRenderer, L"Strrationalism"sv, box1, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"test"sv, box2, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"text"sv, box3, space, fontSize, f);
			menu_.Emplace(&fontRenderer, L"menu"sv, box4, space, fontSize, f);

			Emplace<Scene::VirtualTask>(5.0f, [&] {Engine::Get().Exit(); });
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

static void TestFPSDisplay(Math::Vec2<int> size)
{
	auto engine = Snowing::PlatformImpls::WindowsImpl::MakeEngine(
		L"FPSDisplay",
		size,
		true);
	Font font = LoadFont(LoadAsset(L"Font-zh-CN.fnt"));
	Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
	Scene::Group<> g;
	g.Emplace<Scene::RenderTargetCleaner>(
		&Graphics::Device::MainContext(),
		&Graphics::Device::MainRenderTarget());
	g.Emplace<Scene::FPSDisplay>(&eff, &tech1, &font);
	g.Emplace<Scene::VirtualTask>(0.5f, [] {Engine::Get().Exit(); });

	Engine::Get().RunObject(g);
}

TEST(SceneTest, FPSDisplay)
{
	TestFPSDisplay({ 800,600 });
	TestFPSDisplay({ 400,300 });
	TestFPSDisplay({ 1280,720 });
	TestFPSDisplay({ 960,300 });
}
