#pragma once
#include <cassert>
#include <string_view>
#include <string>

namespace Snowing
{
	template <typename T,typename TFrom>
	T To(TFrom f)
	{
		// Match T and TFrom
		if constexpr (
			std::is_same<std::wstring, T>::value &&
			std::is_same<const wchar_t*, TFrom>::value)
			return std::wstring{ f };

		else if constexpr (
			std::is_same<std::string, T>::value &&
			std::is_same<const char*, TFrom>::value)
			return std::string{ f };

		// Match T
		else if constexpr (std::is_same<std::wstring, T>::value)
			return std::to_wstring(f);

		else if constexpr (std::is_same<std::string, T>::value)
			return std::to_string(f);

		// No Matched
		else
			static_assert(false,"No matched converter.");
	}

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

	template <typename T, typename TFrom>
	constexpr T To(T s)
	{
		return s;
	}


}
