#include "pch.h"

constexpr Snowing::Math::Coordinate2DRect coord
{
	{ -(float)WinSize.x / 2,-(float)WinSize.y / 2 },
	{ (float)WinSize.x / 2, (float)WinSize.y / 2 } };

TEST(HiLevelRendering, SpriteRendering)
{
	auto [win, dev, engine] = MakeEngine(L"SpriteRendering", WinSize, true, { false });

	Snowing::Graphics::SpriteSheet sheet = { 0 };
	auto tex = Snowing::Graphics::LoadSpriteAndSpriteSheet(Snowing::LoadAsset("Sprite.ctx"), sheet);

	Snowing::Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	auto spTech = eff.LoadTechnique("SpriteTest",Sprite::DataLayout);


	auto gpuSheet = Graphics::SpriteRenderer<>::MakeGPUSpriteSheet(sheet);
	auto gpuVBuffer = Graphics::SpriteRenderer<>::MakeGPUVertexBuffer();
	Graphics::SpriteRenderer<> render{ &Graphics::Device::Get().MainContext(),&spTech,coord,&tex,&gpuSheet,&gpuVBuffer };

	Scene::Group<Scene::VirtualTask> group;

	float t = -400;
	std::vector<Sprite> sprites{ 9 };
	for (auto& sp : sprites)
	{
		sp.Position = { t,100 };
		t += 100;

		sp.ImageID = rand() % 8;
	}

	group.Emplace(0.5f, [] {Window::Get().Resize({ 1024,768 }); });
	group.Emplace(1.0f, [] {Window::Get().SetWindowed(false); });
	group.Emplace(1.5f, [] {Window::Get().SetWindowed(true); });
	group.Emplace(2.0f, [] {Window::Get().Resize({ 800,300 }); });


	Sprite sprite;
	sprite.Position = { -400,0 };
	sprite.ImageID = 0;

	Sprite sprite2;
	sprite2.Position = { -400,0 };
	sprite2.ImageID = 5;
	sprite2.Center = { 0,0 };

	Font font = LoadFont(LoadAsset("Font-chs.fnt"));
	Graphics::EffectTech tech1 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);

	Scene::Debug::DebugDisplay spx(
		&tech1, &font, L"Sprite Position X", [&sprite] {
		return std::to_wstring(sprite.Position.x);
	});

	engine.Run([&] {
		engine.Draw([&] {
			auto& g = Device::Get();
			g.MainContext().ClearRenderTarget(g.MainRenderTarget());
			g.MainContext().SetRenderTarget(&g.MainRenderTarget());
			render.Draw(sprites.data(), sprites.size());
			render.DrawToSpriteBuffer(sprite2);
			render.DrawToSpriteBuffer(sprite);
			render.FlushSpriteBuffer();
		});

		group.Update();

		sprite.Position.x += 5.0f;
		sprite.Rotating += 0.01f;
		sprite2.Position.x += 5.0f;
		sprite2.Rotating += 0.08f;
		spx.Update();

		if (sprite.Position.x >= 420)
			engine.Exit();
	});
}


TEST(HiLevelRendering, NoTexSprite)
{
	auto [win, dev, engine] = MakeEngine(L"NoTexSprite", WinSize, true, { false });


	Snowing::Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	auto spTech = eff.LoadTechnique("EffectOnly", Sprite::DataLayout);

	SpriteSheet sheet;
	sheet.Sheet[0] = { 0,0,1,1 };

	auto spriteSheet = Graphics::SpriteRenderer<>::MakeGPUSpriteSheet(sheet);
	auto gpuVBuffer = Graphics::SpriteRenderer<Graphics::Sprite,Graphics::SpriteSheet,8>::MakeGPUVertexBuffer();
	Graphics::SpriteRenderer<Graphics::Sprite, Graphics::SpriteSheet,0> 
		render{ &Graphics::Device::Get().MainContext(),&spTech,coord,nullptr,&spriteSheet,nullptr };

	float t = -350;
	std::vector<Sprite> sprites{ 8 };
	for (auto& sp : sprites)
	{
		sp.Position = { t,100 };
		t += 100;
		sp.Size = { 50.0f,50.0f };
	}

	const std::optional<Snowing::Graphics::Box<size_t>> dataBox =
		Snowing::Graphics::Box<size_t>
	{
		0, sprites.size() * sizeof(Sprite),
		0,1,
		0,1
	};

	gpuVBuffer.UpdateData(Device::Get().MainContext(), sprites.data(), dataBox);

	Scene::Task exit{ 0.1f,[] {Engine::Get().Exit(); } };


	engine.Run([&] {
		engine.Draw([&] {
			auto& g = Device::Get();
			g.MainContext().ClearRenderTarget(g.MainRenderTarget());
			g.MainContext().SetRenderTarget(&g.MainRenderTarget());
			render.Draw(gpuVBuffer,sprites.size());
		});

		exit.Update();
	});
}


TEST(HiLevelRendering, TransparentTextRendering)
{
	auto[win, dev, engine] = MakeEngine(L"TransparentTextRendering", WinSize, true);
	win.SetTransparent();

	Snowing::Graphics::Effect eff{ LoadAsset("HiLevelRendering") };
	auto tech1 = eff.LoadTechnique("FontTestLighting", Sprite::DataLayout);
	auto tech2 = eff.LoadTechnique("FontTestBasic", Sprite::DataLayout);
	auto gpuVBuffer = FontRenderer<>::MakeGPUVertexBuffer();

	std::map<wchar_t, Math::Vec2f> chfix;
	chfix[L'i'] = { -0.2f,0.0f };
	chfix[L's'] = { -0.1f,0.0f };
	chfix[L'm'] = { 0.05f,0.0f };
	chfix[L'r'] = { -0.07f,0.0f };
	chfix[L'a'] = { -0.01f,0.0f };
	chfix[L't'] = { -0.1f,0.0f };
	chfix[L'l'] = { -0.2f,0.0f };
	chfix[L'w'] = { 0.05f,0.0f };
	chfix[L'G'] = { 0.06f,0.0f };
	chfix[L'E'] = { 0.02f,0.0f };
	chfix[L'D'] = { 0.04f,0.0f };

	auto font = LoadFont(LoadAsset("Font-chs.fnt"));
	FontRenderer<> fontRenderer1
	{
		&Graphics::Device::Get().MainContext(),
		&tech1,
		coord,
		&font,
		&gpuVBuffer
	};

	FontRenderer<> fontRenderer2
	{
		&Graphics::Device::Get().MainContext(),
		&tech2,
		coord,
		&font,
		&gpuVBuffer
	};

	FontRenderer<> *fontRenderer = &fontRenderer1;

	const std::wstring s = 
L"#Strrationalism!!!\nTouHou: Dreamming Butterfly is     `so Fun!@~#$%\n\nThis is a text rendering test for Strrationalism Snowing game engine.\n\n汉字输出测试\n，。：？【】正一二三四五爨";

	std::vector<FontSprite> text;

	

	auto setAlign = [&](FontSprite::AlignMode mode,float size = 0.5f)
	{
		FontSprite::SetString<Font>(
			std::wstring_view(s),
			font,
			Math::Vec4f{ -350.0f,-200.0f,700.0f,800.0f },
			Math::Vec2f{ 0.8f,85.0f },
			Math::Vec2f{ size,size },
			text,
			mode);
	};

	setAlign(FontSprite::AlignMode::Left);

	std::chrono::high_resolution_clock clk;
	auto beg = clk.now();

	Scene::Group<Scene::VirtualTask> group;
	group.Emplace(0.1f, [&] { setAlign(FontSprite::AlignMode::Center); });
	group.Emplace(0.2f, [&] { setAlign(FontSprite::AlignMode::Right); });
	group.Emplace(0.3f, [&] { fontRenderer = &fontRenderer2; setAlign(FontSprite::AlignMode::Left, 0.25f); });
	group.Emplace(0.4f, [&] { setAlign(FontSprite::AlignMode::Center,0.25f); });
	group.Emplace(0.5f, [&] { setAlign(FontSprite::AlignMode::Right,0.25f); });

	Input::KeyWatcher mouseLeft
	{
		&Input::Input::Get(),
		Input::MouseKey::Left
	};

	engine.Run([&]{
		engine.Draw([&] {
			auto& g = Device::Get();
			g.MainContext().ClearRenderTarget(g.MainRenderTarget());
			g.MainContext().SetRenderTarget(&g.MainRenderTarget());

			fontRenderer->DrawToSpriteBuffer(text);
			fontRenderer->FlushSpriteBuffer();
		});

		group.Update();
		mouseLeft.Update();
		if (mouseLeft.JustPress())
			Snowing::Log("Mouse Left Just Pressed");
		if (mouseLeft.JustRelease())
			Snowing::Log("Mouse Left Just Released");

		using namespace std::chrono;
		if (clk.now() - beg > 600ms)
		{
			engine.Exit();
		}
	});
}


TEST(HiLevelRendering, PostEffect)
{
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(L"PostEffect", WinSize, true);

	Graphics::PostEffect::PostEffectVertexBuffer vb;
	Graphics::Effect eff{ LoadAsset("PostEffectDemo") };
	auto tech = eff.LoadTechnique("PostEffect", Graphics::PostEffect::PostEffectVertex::DataLayout);

	Graphics::PostEffect post{ &Graphics::Device::Get().MainContext(),&tech };

	Scene::Task task{ 0.25f,[] {Engine::Get().Exit(); } };

	Engine::Get().Run([&] {
		Engine::Get().Draw([&] {
			Graphics::Device::Get().MainContext().SetRenderTarget(
				&Graphics::Device::Get().MainRenderTarget());
			post.Apply();
			task.Update();
		});
	});
}
