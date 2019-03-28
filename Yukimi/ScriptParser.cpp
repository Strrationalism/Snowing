#include "pch.h"
#include "ScriptParser.h"
#include <CSVParser.h>

using namespace Yukimi::Script;

static void writeElement(Line& out, Element e)
{
	for (auto& p : out)
	{
		if (p.index() == 0)
		{
			p = e;
			return;
		}
	}

	throw std::runtime_error{ "Line element is too many." };
}

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
		writeElement(out,LabelElement{ trimString(line) });
		return true;
	}
	else
		return false;
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

static void parseFontStyleList(std::wstring_view styles, Line& out)
{
	FontStyleElement e;
	e.StyleCount = 0;

	auto writeStyle = [&e](std::wstring_view style){
		e.FontStyles[e.StyleCount++] = style;
	};

	while (!styles.empty())
	{
		const auto comma = styles.find(',');

		if (comma == std::wstring_view::npos)
		{
			writeStyle(trimString(styles));
			styles = L"";
		}
		else
		{
			writeStyle(trimString(styles.substr(0, comma)));
			styles = styles.substr(comma);

			if (!styles.empty())
				styles.remove_prefix(1);
		}
	}

	writeElement(out, e);
}

static void parseText(std::wstring_view line, Line& out)
{

	std::wstring_view currentText;

	auto writeTextElement = [&out](std::wstring_view t)
	{
		if (!t.empty())
			writeElement(out, TextElement{ t });
	};

	for (size_t begin = 0, i = 0; i < line.size(); ++i)
	{
		const auto ch = line[i];

		if (ch == '<')
		{
			writeTextElement(currentText);
			currentText = L"";

			const auto right = line.find(' ', i);
			const auto fontStyles = line.substr(i + 1, right - i - 1);
			begin = right + 1;
			parseFontStyleList(trimString(fontStyles), out);
		}
		else if (ch == '>')
		{
			writeTextElement(currentText);
			currentText = L"";

			writeElement(out, FontStylePopElement{});

			begin = i + 1;
		}
		else if (ch == '[')
		{
			writeTextElement(currentText);
			currentText = L"";

			const auto right = line.find(']', i);
			const auto commandLine = line.substr(i + 1, right - i - 1);
			begin = right + 1;
			writeElement(out, parseCommand(trimString(commandLine)));
		}
		else
		{
			currentText = line.substr(begin, i - begin + 1);
		}

	}

	writeTextElement(currentText);
}

static void parseLine(std::wstring_view line, Line& out)
{
	const auto nameSpilter = line.find(':');

	if (nameSpilter != std::wstring_view::npos)
	{
		writeElement(out,CharacterNameElement{ trimString(line.substr(0,nameSpilter)) });
		line = trimString(line.substr(nameSpilter));
		if(!line.empty())
			line.remove_prefix(1);
		line = trimString(line);
	}

	parseText(line,out);
}


static bool parseCommandLine(std::wstring_view line, Line& out)
{
	if (line.front() == '@')
	{
		line.remove_prefix(1);
		writeElement(out,parseCommand(trimString(line)));
		return true;
	}
	return false;
}

Script Yukimi::Script::Parse(const Snowing::Blob* wcharScriptBlob)
{
	Snowing::CSVParser<wchar_t> csv{ wcharScriptBlob };

	Script out;
	out.reserve(1024);

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
				if (!parseCommandLine(currentLine, out.back()))
					parseLine(currentLine, out.back());
		}

		lineNumber++;
		
	} while (csv.NextLine());

	return out;
}
