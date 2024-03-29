#pragma once
#include <cassert>
#include <cstdint>
#include <string_view>

namespace Snowing
{
	class[[nodiscard]] BKDRHash final
	{
	private:
		const uint64_t hash_;

		template<size_t StrLen, typename CharType>
		constexpr static uint64_t Hash(const CharType(&str)[StrLen]) noexcept
		{
			constexpr unsigned int seed = 131;
			uint64_t hash = 0;
			for (size_t i = 0; i < StrLen - 1; i++)
				hash = hash * seed + str[i];
			return hash;
		}

		template<typename CharType>
		static inline uint64_t Hash(const CharType* bytes, size_t size) noexcept
		{
			assert(bytes);
			assert(size);
			constexpr unsigned int seed = 131;
			uint64_t hash = 0;
			for (size_t i = 0; i < size; i++)
				hash = hash * seed + bytes[i];
			return hash;
		}

	public:
		template<size_t StrLen, typename CharType>
		constexpr BKDRHash(const CharType(&str)[StrLen]) noexcept :
			hash_{ Hash(str) }
		{}

		constexpr BKDRHash(uint64_t h) noexcept:
			hash_{ h }
		{}

		template <typename CharType>
		BKDRHash(const CharType* bytes,size_t size) noexcept :
			hash_{ Hash(bytes,size) }
		{
			assert(bytes);
		}

		template <typename CharType>
		BKDRHash(std::basic_string_view<CharType> sv) noexcept :
			hash_{ Hash(sv.data(),sv.size()) }
		{}

		BKDRHash(const BKDRHash&) = default;

		[[nodiscard]]
		constexpr operator uint64_t() const noexcept
		{
			return hash_;
		}
	};
}
