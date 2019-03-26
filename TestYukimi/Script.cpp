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

