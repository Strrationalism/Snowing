#pragma once
#include <cassert>
#include <string_view>
#include <string>
#include <optional>
#include "BKDRHash.h"

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


	template <typename T,typename TFrom>
	std::optional<T> To(const std::optional<TFrom>& f)
	{
		if (f.has_value())
			return std::make_optional(To<T>(*f));
		else
			return std::nullopt;
	}

	template <typename T>
	T To(std::string_view s)
	{
		if constexpr (
			std::is_same<std::wstring, T>::value)
		{
			std::wstring str;
			str.reserve(s.size());
			for (auto ch : s)
				str += static_cast<wchar_t>(ch);
			return str;
		}
		else if constexpr (std::is_same<T, Snowing::BKDRHash>::value)
			return s;
		else
		{
			T t{ 0 };
			s = s.substr(s.find_first_not_of(' '));
			std::from_chars(s.data(), s.data() + s.size(), t);
			return t;
		}
	}

	template <typename T>
	T To(std::string s)
	{
		return To<T>(std::string_view(s));
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

		if constexpr (std::is_same<T, std::string>::value)
			return str;
		else
			return To<T>(std::string_view{ str });
	}

	template <typename T>
	T To(std::wstring s)
	{
		return To<T>(std::wstring_view(s));
	}

	template <typename T, typename TFrom>
	constexpr T To(T s)
	{
		return s;
	}


}

namespace std
{
	template <typename T,typename TString>
	std::basic_string<T> operator + (TString&& str, const std::basic_string_view<T>& str2)
	{
		return std::forward<TString>(str).append(str2.data(),str2.size());
	}
}