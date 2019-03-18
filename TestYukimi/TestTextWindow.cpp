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
public:
	void Update(TextWindow::Charater&) override {}
	void FastFadeIn(TextWindow::Charater&) override {}
	void FadeOut(TextWindow::Charater&) override {}
	void OnHide(TextWindow::Charater&) override {}
	void OnShow(TextWindow::Charater&) override {}
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
	Math::Coordinate2DRect GetTextWindowRectInWindowCoord() const override
	{
		return {
			{-300.0f,-200.0f},
			{300.0f,200.0f}
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
		return std::make_unique<SimpleTextAnimation>();
	}

	void OnHide() override {}
	void OnShow() override {}
};

constexpr TextWindowFontStyle DefaultFontStyle
{
	1.0f,
	"Simple",
	"Simple",
	Math::Vec3f{1,1,1}
};

TEST(TextWindow, ShowText)
{
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(L"TextWindow.ShowText", WinSize, true);
	Device::Get().MainContext().SetRenderTarget(&Device::Get().MainRenderTarget());

	SimpleTextWindowAdapter adapter;

	Scene::Group<> scene;
	scene.Emplace<Scene::RenderTargetCleaner>(
		&Device::Get().MainContext(),&Device::Get().MainRenderTarget(),Math::Vec4f{ 0,0,0,1 });

	scene.Emplace<Scene::VirtualTask>(5.0f, [] {
		Engine::Get().Exit()
	});

	auto textWindow = scene.Emplace<TextWindow>(&adapter);
	textWindow->AppendText(L"今儿天气真不错", DefaultFontStyle,0);

	Engine::Get().RunObject(scene);
}