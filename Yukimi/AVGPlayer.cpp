#include "pch.h"
#include "AVGPlayer.h"
#include <ConditionTask.h>

using namespace Yukimi::Script;

constexpr float ChangePageLimit = 0.05f;

bool Yukimi::AVGPlayer::doElement(const Yukimi::Script::Element& e)
{
	if (auto textElement = std::get_if<TextElement>(&e))
	{
		auto fontStyle = TextWindowFontStyle::Combine(fontStyleStack_.data(), fontStyleStack_.size());
		textWindow_.AppendText(textElement->Text, fontStyle, 0.05f);
		return true;
	}
	else if (auto fontStyleElement = std::get_if<FontStyleElement>(&e))
	{
		fontStyleStackCounts_.push(fontStyleElement->StyleCount);
		for (size_t i = 0; i < fontStyleElement->StyleCount; ++i)
			fontStyleStack_.push_back(*adapter_->GetFontStyle(fontStyleElement->FontStyles[i]));
	}
	else if (auto fontStylePopElement = std::get_if<FontStylePopElement>(&e))
	{
		for (size_t i = 0; i < fontStyleStackCounts_.top(); ++i)
			fontStyleStack_.pop_back();
		fontStyleStackCounts_.pop();
	}
	else if (auto commandElement = std::get_if<CommandElement>(&e))
		adapter_->OnCommand(*commandElement);
	else if (auto charNameElement = std::get_if<CharacterNameElement>(&e))
	{
		fontStyleStack_.push_back(*adapter_->GetCharacterDefaultFontStyle(charNameElement->Name));
		adapter_->OnCharacter(charNameElement->Name);

	}

	return false;
}

void Yukimi::AVGPlayer::runScriptContinuation()
{
	bool isSpeakLine = false;
	while (!isSpeakLine)
	{
		assert(nextLine_ < script_->size());

		// Çå³ýÐÐ×´Ì¬

		characterNameElement_ = nullptr;
		fontStyleStack_.clear();
		fontStyleStack_.push_back(*adapter_->GetDefaultFontStyle());
		while (!fontStyleStackCounts_.empty()) fontStyleStackCounts_.pop();

		const auto& currentLine = (*script_)[nextLine_++];
		for (const auto& element : currentLine)
		{
			const auto ret = doElement(element);
			isSpeakLine = isSpeakLine || ret;
		}
	}
}

void Yukimi::AVGPlayer::Click()
{
	switch (textWindow_.GetState())
	{
	case Yukimi::TextWindow::State::Displaying:
		if (clickLimitTimer_ < ChangePageLimit)
			break;
		clickLimitTimer_ = 0;
		textWindow_.FadeClear();
		{
			const auto condition = [this] { 
				return textWindow_.GetState() == Yukimi::TextWindow::State::EmptyTextWindow;
			};

			const auto task = [this] { runScriptContinuation(); };

			Emplace<Snowing::Scene::ConditionTask<decltype(condition), decltype(task)>>(condition, task);
		}
		break;
	case Yukimi::TextWindow::State::FadingInText:
		textWindow_.FastFadeIn();
		break;
	case Yukimi::TextWindow::State::EmptyTextWindow:
		break;
	case Yukimi::TextWindow::State::FadingOutText:
		break;
	default:
		throw std::runtime_error{ "Unknown text window state." };
	};
}

void Yukimi::AVGPlayer::Goto(std::wstring_view labelName)
{
	for (nextLine_ = 0; nextLine_ < script_->size(); ++nextLine_)
		if (auto labelElement = std::get_if<LabelElement>(&(*script_)[nextLine_][0]))
			if (labelElement->LabelName == labelName)
				return;
	throw std::runtime_error{ "Can not find label in story script." };
}

Yukimi::AVGPlayer::AVGPlayer(
	const Yukimi::Script::Script* script, 
	TextWindow::TextWindowUserAdapter* textWindowAdapter,
	AVGPlayerUserAdapter* avgPlayerAdapter):
	textWindow_{ textWindowAdapter },
	script_{ script },
	adapter_{ avgPlayerAdapter }
{
	runScriptContinuation();
}

bool Yukimi::AVGPlayer::Update()
{
	clickLimitTimer_ += Snowing::Engine::Get().DeltaTime();

	Snowing::Scene::Group<>::Update();
	textWindow_.Update();
	return true;
}
