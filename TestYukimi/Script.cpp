#include "pch.h"
#include <ScriptParser.h>

#define PARSE(code) \
std::wstring_view w = code;\
Snowing::Blob b{ w.size() * sizeof(wchar_t) };\
for (size_t i = 0; i < w.size() * sizeof(wchar_t); ++i)\
{\
const uint8_t* byte = reinterpret_cast<const uint8_t*>(code); \
* b.Get<uint8_t*>(i) = byte[i]; \
}\
auto s = Yukimi::Script::Parse(&b);
	
using namespace Yukimi::Script;

template <typename T,typename U>
void SAssert(const T& variant, const U& b)
{
	Assert(std::get<U>(variant), b);
}

TEST(Script, Label)
{
	PARSE(
		L"\n"
		" -  Label1	\n"
		"FuckAllOver\n"
		"- Label2	\n"
		"Fucker\n"
		"	  -Label3	#CNM	NMSL	\n"
		"  \n"
		"");

	SAssert(s[0][0], LabelElement{ L"Label1" });
	SAssert(s[2][0], LabelElement{ L"Label2" });
	SAssert(s[4][0], LabelElement{ L"Label3" });
}

TEST(Script, CommandLine)
{
	PARSE(
		L"\n"
		" @fuck stupid1 stupid2 stupid3\n"
		"@  fuck2\n"
		"   @    fuck3       stupid\n"
		"");

	SAssert(s[0][0], CommandElement{ L"fuck",{L"stupid1",L"stupid2",L"stupid3"},3 });
	SAssert(s[1][0], CommandElement{ L"fuck2",{},0 });
	SAssert(s[2][0], CommandElement{ L"fuck3",{L"stupid"},1 });
}

TEST(Script, CharaterName)
{
	PARSE(
		L"\n"
		" D : Szz\n"
		" SZZ :ASS\n"
		"D: Excp\n"
		"");

	SAssert(s[0][0], CharacterNameElement{ L"D" });
	SAssert(s[1][0], CharacterNameElement{ L"SZZ" });
	SAssert(s[2][0], CharacterNameElement{ L"D"});
}


TEST(Script, TextLine)
{
	PARSE(
		L"Szz[s 123] Default\n"
		" D : Sz[NMSL]z\n"
		" SZZ :A[Stupid 1 2 3 4]S[Stupid2]S\n"
		"");

	SAssert(s[0][1], TextElement{ L"Szz" });
	SAssert(s[0][2], CommandElement{ L"s",{L"123"},1 });
	SAssert(s[0][3], TextElement{ L" Default" });
	SAssert(s[0][4], Nop{ });

	SAssert(s[1][0], CharacterNameElement{ L"D" });
	SAssert(s[1][1], TextElement{ L"Sz" });
	SAssert(s[1][2], CommandElement{ L"NMSL",{},0 });
	SAssert(s[1][3], TextElement{ L"z" });
	SAssert(s[1][4], Nop{ });

	SAssert(s[2][0], CharacterNameElement{ L"SZZ" });
	SAssert(s[2][1], TextElement{ L"A" });
	SAssert(s[2][2], CommandElement{ L"Stupid",{L"1",L"2",L"3",L"4"},4 });
	SAssert(s[2][3], TextElement{ L"S" });
	SAssert(s[2][4], CommandElement{ L"Stupid2",{},0 });
	SAssert(s[2][5], TextElement{ L"S" });
	SAssert(s[2][6], Nop{ });
}

TEST(Script, TextLine2)
{
	PARSE(
		L"Daiyousei: <s,b,x,y [NMSL 1 2 3] This is an text <red test>>.\n"
		L"Lyt: WYDSLNM,NMSL,WCNMB [I 1 2 3] <a,b,z,y wwwww[ccc]>.\n"
		L"<Stupid,Stupid stup[s 10]id>\n"
	);

	SAssert(s[0][0], CharacterNameElement{ L"Daiyousei" });
	SAssert(s[0][1], FontStyleElement{ {L"s",L"b",L"x",L"y"},4 });
	SAssert(s[0][2], CommandElement{ L"NMSL",{L"1",L"2",L"3"},3 });
	SAssert(s[0][3], TextElement{ L" This is an text " });
	SAssert(s[0][4], FontStyleElement{ {L"red"},1 });
	SAssert(s[0][5], TextElement{ L"test" });
	SAssert(s[0][6], FontStylePopElement{ });
	SAssert(s[0][7], FontStylePopElement{ });
	SAssert(s[0][8], TextElement{ L"." });
	SAssert(s[0][9], Nop{ });

	SAssert(s[1][0], CharacterNameElement{ L"Lyt" });
	SAssert(s[1][1], TextElement{ L"WYDSLNM,NMSL,WCNMB " });
	SAssert(s[1][2], CommandElement{ L"I",{L"1",L"2",L"3"},3 });
	SAssert(s[1][3], TextElement{ L" " });
	SAssert(s[1][4], FontStyleElement{ {L"a",L"b",L"z",L"y"},4 });
	SAssert(s[1][5], TextElement{ L"wwwww" });
	SAssert(s[1][6], CommandElement{ L"ccc",{},0 });
	SAssert(s[1][7], FontStylePopElement{ });
	SAssert(s[1][8], TextElement{ L"." });
	SAssert(s[1][9], Nop{ });

	SAssert(s[2][1], FontStyleElement{ {L"Stupid",L"Stupid"},2 });
	SAssert(s[2][2], TextElement{ L"stup" });
	SAssert(s[2][3], CommandElement{ L"s",{L"10"},1 });
	SAssert(s[2][4], TextElement{ L"id" });
	SAssert(s[2][5], FontStylePopElement{ });
	SAssert(s[2][6], Nop{ });

	constexpr auto p = sizeof(Line);
}

