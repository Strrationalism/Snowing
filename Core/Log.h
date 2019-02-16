#pragma once
#include <sstream>

namespace Snowing
{
	template <typename CharType = char,typename ... TArgs>
	void Log(const TArgs&... args)
	{
#ifdef _DEBUG
		static std::basic_stringstream<CharType> stream;
		constexpr CharType ch[] = { '\0' };
		stream.str(ch);
		((stream << args << ' '), ...);
		PlatformImpls::Log(stream.str().c_str());
#endif // _DEBUG
	}
}
