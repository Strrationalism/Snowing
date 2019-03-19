#include "pch.h"
#include <TextWindowFontStyle.h>
#include <TextWindow.h>

using namespace Yukimi;

TEST(TextWindow, FontStyleLoadingCombine)
{
	const TextWindowFontStyle styles[] =
	{
		TextWindowFontStyle::LoadFromProp(LoadAsset("TextWindowFontBase.txt")),
		TextWindowFontStyle::LoadFromProp(LoadAsset("TextWindowFontDeriv.txt"))
	};

	const auto style3 = TextWindowFontStyle::Combine(styles, 2);

	Assert(*style3.Color, styles[1].Color);
	Assert(*style3.Size, styles[1].Size);

	Assert(*style3.ShaderID, styles[0].ShaderID);
	Assert(*style3.AnimationID, styles[0].AnimationID);
}

constexpr Math::Coordinate2DRect WinCoord{
	{-400.0f,-300.0f},
	{400.0f,300.0f}
};

class SimpleTextAnimation final : public TextWindow::TextAnimation
{
private:
	const float orgSize_;
	bool visible_ = true;
public:
	SimpleTextAnimation(float orgSize) : orgSize_{ orgSize } {}
	void Update(TextWindow::Charater& c) override
	{
		if (c.SinceFadeInTime > -0.1f && c.SinceFadeInTime < 0)
		{
			c.Sprite.Sprite.Color.w = visible_ ? 10 * (c.SinceFadeInTime + 0.1f) : 0;
			
			auto size = (1 - c.Sprite.Sprite.Color.w) * orgSize_ * 1.5f + orgSize_;
			c.Sprite.Sprite.Size = { size,size };
		}
		else if (c.SinceFadeInTime >= 0)
		{
			c.Sprite.Sprite.Color.w = visible_;
			c.Sprite.Sprite.Size = { orgSize_,orgSize_ };
		}
	}

	AnimationState GetState(const TextWindow::Charater& c) const override
	{
		if (c.SinceFadeInTime <= -0.1f)
			return AnimationState::Ready;
		else if (c.SinceFadeInTime > -0.1f && c.SinceFadeInTime < 0)
			return AnimationState::FadingIn;
		else if (c.SinceFadeInTime >= 0)
			return AnimationState::Displaying;
		throw std::exception{};
	}

	void FastFadeIn(TextWindow::Charater& c) override 
	{
		c.SinceFadeInTime = 1;
	}

	void FadeOut(TextWindow::Charater&) override {}

	void OnHide(TextWindow::Charater&) override 
	{
		visible_ = false;
	}

	void OnShow(TextWindow::Charater&) override
	{
		visible_ = true;
	}
};

class SimpleTextWindowAdapter final : public TextWindow::TextWindowUserAdapter
{
private:
	Effect effect_{ LoadAsset("TextWindowShaders.cso") };
	EffectTech basicFont_ = effect_.LoadTechnique("BasicFont", Sprite::DataLayout);
	Font font_ = LoadFont(LoadAsset("Font-chs.fnt"));
	Buffer vb_ = TextWindow::FontRenderer::MakeGPUVertexBuffer();

	TextWindow::FontRenderer basicFr_
	{
		&Graphics::Device::MainContext(),
		&basicFont_,
		WinCoord,
		&font_,
		&vb_
	};

public:
	Math::Vec4f GetTextWindowBox() const override
	{
		return {
			-300.0f,-200.0f,
			300.0f,200.0f
		};
	}

	TextWindow::FontRenderer* GetFontRendererByShaderName(Snowing::BKDRHash) override
	{
		return &basicFr_;
	}

	void FlushDrawCall() override
	{
		basicFr_.FlushSpriteBuffer();
	}

	const Snowing::Graphics::Font& GetFont() const override
	{
		return font_;
	}

	std::unique_ptr<TextWindow::TextAnimation> CreateAnimationByName(
		TextWindow::Charater& ch, Snowing::BKDRHash) override
	{
		ch.Sprite.Sprite.Color.w = 0;
		return std::make_unique<SimpleTextAnimation>(ch.Sprite.Sprite.Size.x);
	}

	void OnHide() override 
	{

	}

	void OnShow() override {}
};

constexpr TextWindowFontStyle DefaultFontStyle
{
	1.0f,
	"Simple",
	"Simple",
	Math::Vec3f{1.0f,1.0f,1.0f}
};

auto CreateShowTextScene(TextWindow::TextWindowUserAdapter& adapter)
{
	Scene::Group<> scene;
	scene.Emplace<Scene::RenderTargetCleaner>(
		&Device::Get().MainContext(), &Device::Get().MainRenderTarget(), Math::Vec4f{ 0,0,0,1 });

	scene.Emplace<Scene::VirtualTask>(5.0f, [] {
		Engine::Get().Exit();
	});

	auto textWindow = scene.Emplace<TextWindow>(&adapter);
	textWindow->AppendText(L"StrrationalismStrrationalismStrrationalismStrrationalism\nStrrationalism\nStrrationalism  Strrationalism Strrationalism  Strrationalism is so good.", DefaultFontStyle, 0.005f);

	TextWindowFontStyle bigFont;
	bigFont.Size = 1.5f;
	bigFont = TextWindowFontStyle::Combine(DefaultFontStyle, bigFont);

	TextWindowFontStyle smallFont;
	smallFont.Size = 0.8f;
	smallFont = TextWindowFontStyle::Combine(DefaultFontStyle, smallFont);

	for (int i = 0; i < 3; ++i)
	{
		textWindow->AppendText(L"\nStrrationalism", DefaultFontStyle, 0.03f);
		textWindow->AppendText(L"Strrationalism", bigFont, 0.03f);
		textWindow->AppendText(L"Strrationalism", smallFont, 0.03f);
	}

	return scene;
}

TEST(TextWindow, ShowText)
{
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(L"TextWindow.ShowText", WinSize, true);
	Device::Get().MainContext().SetRenderTarget(&Device::Get().MainRenderTarget());

	SimpleTextWindowAdapter adapter;
	Engine::Get().RunObject(CreateShowTextScene(adapter));
}

TEST(TextWindow, FastFadeIn)
{
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(L"TextWindow.FastFadeIn", WinSize, true);
	Device::Get().MainContext().SetRenderTarget(&Device::Get().MainRenderTarget());

	SimpleTextWindowAdapter adapter;
	auto scene = CreateShowTextScene(adapter);

	scene.Emplace<Scene::VirtualTask>(1.0f, [&scene] {
		scene.IterType<TextWindow>([](TextWindow & w) {
			w.FastFadeIn();
		});
	});

	Engine::Get().RunObject(scene);
}

TEST(TextWindow, SetVisible)
{
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(L"TextWindow.SetVisible", WinSize, true);
	Device::Get().MainContext().SetRenderTarget(&Device::Get().MainRenderTarget());

	SimpleTextWindowAdapter adapter;
	auto scene = CreateShowTextScene(adapter);

	scene.Emplace<Scene::VirtualTask>(0.5f, [&scene] {
		scene.IterType<TextWindow>([](TextWindow & w) {
			w.SetVisible(false);
		});
	});

	scene.Emplace<Scene::VirtualTask>(1.0f, [&scene] {
		scene.IterType<TextWindow>([](TextWindow & w) {
			w.SetVisible(true);
		});
	});

	scene.Emplace<Scene::VirtualTask>(1.5f, [&scene] {
		scene.IterType<TextWindow>([](TextWindow & w) {
			w.SetVisible(false);
		});
	});

	scene.Emplace<Scene::VirtualTask>(2.0f, [&scene] {
		scene.IterType<TextWindow>([](TextWindow & w) {
			w.SetVisible(true);
		});
	});

	Engine::Get().RunObject(scene);
}