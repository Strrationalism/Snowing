#pragma once
#include <vector>
#include <array>
#include <variant>
#include <Blob.h>

namespace Yukimi::Script
{
	constexpr size_t MaxArguments = 4;
	constexpr size_t MaxFontStyles = 4;
	constexpr size_t MaxElementInLine = 16;

	struct Nop {};
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
		std::array<std::wstring_view, MaxFontStyles> FontStyles;
		std::wstring_view Charater;
		std::wstring_view Text;
	};

	struct LabelElement
	{
		std::wstring_view LabelName;

		inline bool operator != (LabelElement b) const { return LabelName != b.LabelName; }
	};

	using Element = std::variant<Nop, LabelElement, CommandElement, TextElement>;
	using Line = std::array<Element, MaxElementInLine>;
	using Script = std::vector<Line>;

	Script Parse(const Snowing::Blob* wcharScriptBlob);

}