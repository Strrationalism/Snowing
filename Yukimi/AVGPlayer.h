#pragma once
#include <stack>
#include <Object.h>
#include <ScriptParser.h>
#include <TextWindow.h>
#include <AVGInput.h>

namespace Yukimi
{
	class AVGPlayer final : public Snowing::Scene::Group<>
	{
	public:
		class AVGPlayerUserAdapter
		{
		public:
			virtual const TextWindowFontStyle* GetDefaultFontStyle() const = 0;
			virtual const TextWindowFontStyle* GetFontStyle(std::wstring_view name) const = 0;
			virtual const TextWindowFontStyle* GetCharacterDefaultFontStyle(std::wstring_view name) const = 0;

			virtual std::function<bool()> OnCommand(const Script::CommandElement& command) = 0;
			virtual void OnCharacter(const std::wstring_view name) = 0;
		};

	private:
		TextWindow textWindow_;
		AVGPlayerUserAdapter* adapter_;

		const Script::Script* const script_;

		// 行状态，每行结束需要重置
		std::vector<TextWindowFontStyle> fontStyleStack_;
		std::stack<size_t> fontStyleStackCounts_;

		Script::CharacterNameElement* characterNameElement_;



		// 如果当前行结束后需要停止，则返回true
		bool doElement(const Script::Element&);
		void runScriptContinuation();

		bool waitingForCommand_ = false;
		
		size_t nextLine_ = 0;

		float clickLimitTimer_ = 0;

	public:
		AVGPlayer(
			const Script::Script* script,
			TextWindow::TextWindowUserAdapter* textWindowAdapter,
			AVGPlayerUserAdapter * avgPlayerAdapter);

		bool Update() override;
		void Click();

		void Goto(std::wstring_view labelName);

		TextWindow& GetTextWindow();
	};
}