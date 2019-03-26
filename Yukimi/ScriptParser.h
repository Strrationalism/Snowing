#pragma once
#include <vector>
#include <array>
#include <variant>
#include <Blob.h>

namespace Yukimi::Script
{
	constexpr size_t MaxArguments = 4;
	constexpr size_t MaxFontStyles = 4;
	constexpr size_t MaxElementInLine = 32;

	struct Nop {
		constexpr bool operator != (Nop p) const { return false; }
	};

	struct CommandElement
	{
		std::wstring_view Command;
		std::array<std::wstring_view, MaxArguments> Arguments;
		size_t ArgCount;

		inline bool operator != (CommandElement b) const
		{
			size_t i;
			for (i = 0; i < ArgCount; ++i)
				if (Arguments[i] != b.Arguments[i])
					return true;

			for (; i < MaxArguments; ++i)
				if (!b.Arguments[i].empty() || !Arguments[i].empty())
					return true;

			return 
				Command != b.Command ||
				ArgCount != b.ArgCount;
		}
	};

	struct TextElement
	{
		std::wstring_view Text;

		inline bool operator != (TextElement b) const { return Text != b.Text; }
	};

	struct FontStyleElement
	{
		std::array<std::wstring_view, MaxFontStyles> FontStyles;
		size_t StyleCount;

		inline bool operator != (FontStyleElement b) const
		{
			size_t i;
			for (i = 0; i < StyleCount; ++i)
				if (FontStyles[i] != b.FontStyles[i])
					return true;

			for (; i < MaxArguments; ++i)
				if (!b.FontStyles[i].empty() || !FontStyles[i].empty())
					return true;

			return StyleCount != b.StyleCount;
		}
	};

	struct FontStylePopElement {
		constexpr bool operator != (FontStylePopElement p) const { return false; }
	};

	struct CharaterNameElement
	{
		std::wstring_view Name;

		inline bool operator != (CharaterNameElement b) const { return Name != b.Name; }
	};

	struct LabelElement
	{
		std::wstring_view LabelName;

		inline bool operator != (LabelElement b) const { return LabelName != b.LabelName; }
	};

	using Element = std::variant<
		Nop, 
		LabelElement,
		CommandElement,
		TextElement,
		CharaterNameElement,
		FontStylePopElement,
		FontStyleElement>;

	using Line = std::array<Element, MaxElementInLine>;
	using Script = std::vector<Line>;

	Script Parse(const Snowing::Blob* wcharScriptBlob);

}