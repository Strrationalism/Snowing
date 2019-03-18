#include "pch.h"
#include "TextWindow.h"

using namespace Snowing::Graphics;

void Yukimi::TextWindow::appendCharater(wchar_t ch, const TextWindowFontStyle& style, float wait)
{
	text_.emplace_back();
	auto& p = text_.back();
	
	p.Sprite.SetCharater(userAdapter_->GetFont(), ch);
	p.LifeTime = 0;
	p.WaitTime = wait;
	p.SinceFadeInTime = -wait;	//TODO:此计算不正确，目前仅用于Debug
	
	assert(style.AnimationID.has_value());
	assert(style.ShaderID.has_value());
	p.Animation = userAdapter_->CreateAnimationByName(p,*style.AnimationID);
	p.Renderer = userAdapter_->GetFontRendererByShaderName(*style.ShaderID);
}

Yukimi::TextWindow::TextWindow(TextWindowUserAdapter* userAdapter):
	userAdapter_{ userAdapter }
{
}

void Yukimi::TextWindow::Clear()
{
	text_.clear();
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

