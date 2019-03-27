#include "pch.h"
#include "TextWindow.h"

using namespace Snowing::Graphics;

constexpr float MagicFontSize = 0.25f;

void Yukimi::TextWindow::appendCharater(wchar_t ch, const TextWindowFontStyle& style, float wait)
{
	assert(
		GetState() == State::EmptyTextWindow ||
		GetState() == State::FadingInText ||
		GetState() == State::Displaying);

	auto pos = typer_.Type(ch, *style.Size);

	currentTimeLineEnd_ += wait;

	if (ch != '\n' && ch != ' ')
	{
		text_.emplace_back();
		auto& p = text_.back();

		p.Sprite.SetCharater(userAdapter_->GetFont(), ch);
		p.LifeTime = 0;
		p.WaitTime = wait;
		p.SinceFadeInTime = -currentTimeLineEnd_;	//TODO:此计算不正确，目前仅用于Debug

		assert(style.AnimationID.has_value());
		assert(style.ShaderID.has_value());
		
		p.Renderer = userAdapter_->GetFontRendererByShaderName(*style.ShaderID);

		p.Sprite.Sprite.Color.x = style.Color->x;
		p.Sprite.Sprite.Color.y = style.Color->y;
		p.Sprite.Sprite.Color.z = style.Color->z;

		assert(style.Size.has_value());
		p.Sprite.Sprite.Position = pos;
		p.Sprite.Sprite.Size = { *style.Size * MagicFontSize,*style.Size * MagicFontSize };

		p.Sprite.Sprite.Center = { 1.0f,1.0f };

		p.Animation = userAdapter_->CreateAnimationByName(p, *style.AnimationID);
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
	},
	currentTimeLineEnd_{ 0 }
{
}

void Yukimi::TextWindow::Clear()
{
	fadingOut_ = false;
	text_.clear();
	typer_.Reset();

	currentTimeLineEnd_ = 0;
}

void Yukimi::TextWindow::FadeClear()
{
	fadingOut_ = true;
	for (auto& p : text_)
	{
		const auto state = p.Animation->GetState(p);
		if (
			state == TextAnimation::AnimationState::Ready ||
			state == TextAnimation::AnimationState::FadingIn ||
			state == TextAnimation::AnimationState::Displaying)
			p.Animation->FadeOut(p);
	}
}


void Yukimi::TextWindow::AppendText(std::wstring_view text, const TextWindowFontStyle& style, float wait)
{
	for (auto p : text)
		appendCharater(p, style, wait);
}

void Yukimi::TextWindow::FastFadeIn()
{
	for (auto& p : text_)
	{
		const auto state = p.Animation->GetState(p);
		if (
			state == TextAnimation::AnimationState::FadingIn ||
			state == TextAnimation::AnimationState::Ready)
			p.Animation->FastFadeIn(p);
	}
}

void Yukimi::TextWindow::SetVisible(bool vis)
{
	if (visible_ != vis)
	{
		visible_ = vis;
		
		for (auto& p : text_)
			p.Animation->SetVisible(p,vis);
	}
}

bool Yukimi::TextWindow::Update()
{
	float deltaTime = Snowing::Engine::Get().DeltaTime();

	currentTimeLineEnd_ = std::min(0.0f, currentTimeLineEnd_ - deltaTime);

	for (auto& p : text_)
	{
		p.LifeTime += deltaTime;
		p.SinceFadeInTime += deltaTime;
		
		p.Animation->Update(p);
		p.Renderer->DrawToSpriteBuffer(p.Sprite);
	}

	userAdapter_->FlushDrawCall();

	if (fadingOut_)
	{
		if (std::all_of(text_.begin(), text_.end(), [](const Charater & c) {
			return c.Animation->GetState(c) == TextAnimation::AnimationState::Killed;
		}))
			Clear();
	}

	return true;
}

Yukimi::TextWindow::State Yukimi::TextWindow::GetState() const
{
	if (fadingOut_)
		return Yukimi::TextWindow::State::FadingOutText;
	else if (text_.empty())
		return Yukimi::TextWindow::State::EmptyTextWindow;
	else
	{
		if (std::any_of(text_.begin(), text_.end(),
			[](const Charater & ch) {
			return 
				ch.Animation->GetState(ch) == TextAnimation::AnimationState::FadingIn ||
				ch.Animation->GetState(ch) == TextAnimation::AnimationState::Ready;
		}))
			return Yukimi::TextWindow::State::FadingInText;

		else if (std::all_of(text_.begin(), text_.end(),
			[](const Charater & ch) {
			return ch.Animation->GetState(ch) == TextAnimation::AnimationState::Displaying;
		}))
			return Yukimi::TextWindow::State::Displaying;
		else
			throw std::runtime_error{ "TextWindow in unknown state." };
	}
}

Yukimi::TextWindow::BasicAnimation::BasicAnimation(Ch& ch)
{
	ch.Sprite.Sprite.Color.w = 0;
}

void Yukimi::TextWindow::BasicAnimation::Update(Ch& ch)
{
	if (ch.SinceFadeInTime > 0 && state_ != State::Killed && vis_)
	{
		FastFadeIn(ch);
	}
	else if (state_ == State::Displaying)
		ch.Sprite.Sprite.Color.w = 1;
	else
		ch.Sprite.Sprite.Color.w = 0;
}

Yukimi::TextWindow::TextAnimation::AnimationState Yukimi::TextWindow::BasicAnimation::GetState(const Ch&) const
{
	return state_;
}

void Yukimi::TextWindow::BasicAnimation::FastFadeIn(Ch& ch)
{
	ch.Sprite.Sprite.Color.w = 1;
	state_ = State::Displaying;
}

void Yukimi::TextWindow::BasicAnimation::FadeOut(Ch&)
{
	state_ = State::Killed;
}

void Yukimi::TextWindow::BasicAnimation::SetVisible(Ch&, bool vis)
{
	vis_ = vis;
}
