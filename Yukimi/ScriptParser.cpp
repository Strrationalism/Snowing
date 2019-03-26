#include "pch.h"
#include "ScriptParser.h"
#include <CSVParser.h>

using namespace Yukimi::Script;

static std::wstring_view trimString(std::wstring_view sv)
{
	auto isUsableChar = [](wchar_t w)
	{
		return w != ' ' && w != '\t' && w != '\r' && w != '\0' && w != '\n' && w != 'x0B';
	};

	auto removeOne = [&sv, isUsableChar](auto ch,auto remover) {
		if (sv.empty())
			return false;
		else
		{
			if (!isUsableChar(ch()))
			{
				remover();
				return true;
			}
			else
				return false;
		}
	};
	while (removeOne([&sv] {return sv.front(); }, [&sv] { sv.remove_prefix(1); }));
	while (removeOne([&sv] {return sv.back(); }, [&sv] { sv.remove_suffix(1); }));
	return sv;
}

static bool parseLabel(std::wstring_view line, Line& out)
{
	if (line.front() == '-')
	{
		line.remove_prefix(1);
		out[0] = LabelElement{ trimString(line) };
		return true;
	}
	else
		return false;
}

static bool parseLine(std::wstring_view line, Line& out)
{
	// TODO: 在这里插入产生TextElement的函数
	return true;
}

static CommandElement parseCommand(std::wstring_view s)
{
	CommandElement e;

	const auto firstSpace = s.find(' ');

	e.Command = firstSpace == std::wstring::npos ? s : s.substr(0, firstSpace);
	e.ArgCount = 0;

	std::wstring_view arguments = 
		firstSpace == std::wstring::npos ? L"" : trimString(s.substr(firstSpace, s.length() - firstSpace));


	while (!arguments.empty())
	{
		assert(e.ArgCount <= MaxArguments);

		const auto argSpilter = arguments.find(' ');
		if (argSpilter != std::wstring_view::npos)
		{
			e.Arguments[e.ArgCount++] = trimString(arguments.substr(0, argSpilter));
			arguments = trimString(arguments.substr(argSpilter));
		}
		else
		{
			e.Arguments[e.ArgCount++] = trimString(arguments);
			arguments = L"";
		}
	}

	return e;
}

static bool parseCommandLine(std::wstring_view line, Line& out)
{
	if (line.front() == '@')
	{
		line.remove_prefix(1);
		out[0] = parseCommand(trimString(line));
		return true;
	}
	return false;
}

Script Yukimi::Script::Parse(const Snowing::Blob* wcharScriptBlob)
{
	Snowing::CSVParser<wchar_t> csv{ wcharScriptBlob };

	Script out;

	size_t lineNumber = 0;

	do
	{
		const auto currentLineNotTrimed = csv.CurrentLine();
		const auto currentLine = trimString(currentLineNotTrimed);

		if (!currentLine.empty())
		{
			out.emplace_back();

			for (auto& ele : out.back())
				ele = Nop{};

			if (!parseLabel(currentLine, out.back()))
				if(!parseCommandLine(currentLine,out.back()))
					if (!parseLine(currentLine, out.back()))
						throw std::runtime_error("In line " + std::to_string(lineNumber) + " can not parse as script.");
		}

		lineNumber++;
		
	} while (csv.NextLine());

	return out;
}
