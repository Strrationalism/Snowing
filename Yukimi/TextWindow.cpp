#include "pch.h"
#include "TextWindow.h"

using namespace Snowing::Graphics;

constexpr float MagicFontSize = 0.25f;

void Yukimi::TextWindow::appendCharater(wchar_t ch, const TextWindowFontStyle& style, float wait)
{
	auto pos = typer_.Type(ch, *style.Size);

	if (ch != '\n' && ch != ' ')
	{
		text_.emplace_back();
		auto& p = text_.back();

		p.Sprite.SetCharater(userAdapter_->GetFont(), ch);
		p.LifeTime = 0;
		p.WaitTime = wait;
		p.SinceFadeInTime = -wait;	//TODO:此计算不正确，目前仅用于Debug

		assert(style.AnimationID.has_value());
		assert(style.ShaderID.has_value());
		p.Animation = userAdapter_->CreateAnimationByName(p, *style.AnimationID);
		p.Renderer = userAdapter_->GetFontRendererByShaderName(*style.ShaderID);

		p.Sprite.Sprite.Color.x = style.Color->x;
		p.Sprite.Sprite.Color.y = style.Color->y;
		p.Sprite.Sprite.Color.z = style.Color->z;

		assert(style.Size.has_value());
		p.Sprite.Sprite.Position = pos;
		p.Sprite.Sprite.Size = { *style.Size * MagicFontSize,*style.Size * MagicFontSize };

		p.Sprite.Sprite.Center = { 1.0f,1.0f };
	}
}

Yukimi::TextWindow::TextWindow(TextWindowUserAdapter* userAdapter) :
	userAdapter_{ userAdapter },
	typer_
	{
		&userAdapter->GetFont(),
		userAdapter->GetTextWindowBox(),
		{MagicFontSize,MagicFontSize},
		{0.9f,180.0f},
		&fix_
	}
{
}

void Yukimi::TextWindow::Clear()
{
	text_.clear();
	typer_.Reset();
}

void Yukimi::TextWindow::AppendText(std::wstring_view text, const TextWindowFontStyle& style, float wait)
{
	for (auto p : text)
		appendCharater(p, style, wait);
}

bool Yukimi::TextWindow::Update()
{
	float deltaTime = Snowing::Engine::Get().DeltaTime();
	for (auto& p : text_)
	{
		p.LifeTime += deltaTime;
		p.SinceFadeInTime += deltaTime;
		
		p.Animation->Update(p);
		p.Renderer->DrawToSpriteBuffer(p.Sprite);
	}

	userAdapter_->FlushDrawCall();

	return true;
}

