#include "pch.h"
#include "AVGPlayer.h"
#include <ConditionTask.h>

using namespace Yukimi::Script;

constexpr float ChangePageLimit = 0.05f;

bool Yukimi::AVGPlayer::doElement(const Yukimi::Script::Element& e)
{
	if (auto textElement = std::get_if<TextElement>(&e))
	{
		textWindow_->AppendText(textElement->Text, *defaultStyle_, 0.05f);
		return true;
	}
	else
		return false;
}

void Yukimi::AVGPlayer::runScriptContinuation()
{
	bool isSpeakLine = false;
	while (!isSpeakLine)
	{
		assert(nextLine_ < script_->size());

		const auto& currentLine = (*script_)[nextLine_++];
		for (const auto& element : currentLine)
			isSpeakLine = isSpeakLine || doElement(element);
	}
}

void Yukimi::AVGPlayer::Click()
{
	switch (textWindow_->GetState())
	{
	case Yukimi::TextWindow::State::Displaying:
		if (clickLimitTimer_ < ChangePageLimit)
			break;
		clickLimitTimer_ = 0;
		textWindow_->FadeClear();
		{
			const auto condition = [this] { 
				return textWindow_->GetState() == Yukimi::TextWindow::State::EmptyTextWindow;
			};

			const auto task = [this] { runScriptContinuation(); };

			Emplace<Snowing::Scene::ConditionTask<decltype(condition), decltype(task)>>(condition, task);
		}
		break;
	case Yukimi::TextWindow::State::FadingInText:
		textWindow_->FastFadeIn();
		break;
	case Yukimi::TextWindow::State::EmptyTextWindow:
		break;
	case Yukimi::TextWindow::State::FadingOutText:
		break;
	default:
		throw std::runtime_error{ "Unknown text window state." };
	};
}

Yukimi::AVGPlayer::AVGPlayer(
	const Yukimi::Script::Script* script, 
	TextWindow::TextWindowUserAdapter* textWindowAdapter,
	const TextWindowFontStyle* defaultStyle):
	textWindow_{ Emplace<TextWindow>(textWindowAdapter) },
	script_{ script },
	defaultStyle_{ defaultStyle }
{
	runScriptContinuation();
}

bool Yukimi::AVGPlayer::Update()
{
	clickLimitTimer_ += Snowing::Engine::Get().DeltaTime();

	Snowing::Scene::Group<>::Update();
	return true;
}

