#pragma once
#include <cassert>
#include <string_view>

namespace Snowing
{
	template <typename T>
	T To(std::string_view s)
	{
		T t{ 0 };
		std::from_chars(s.data(), s.data() + s.size(), t);
		return t;
	}

	template <typename T>
	T To(std::wstring_view s)
	{
		constexpr size_t MaxSize = 64;
		char str[MaxSize];
		assert(s.size() + 1 <= MaxSize);

		for (size_t i = 0; i < s.size(); ++i)
			str[i] = static_cast<char>(s[i]);

		str[s.size()] = L'\0';
		return To<T>(std::string_view{ str });
	}
}
