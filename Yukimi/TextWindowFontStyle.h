#pragma once
#include <Vec3.h>
#include <PropParser.h>
#include <BKDRHash.h>
#include <optional>

namespace Yukimi
{
	struct[[nodiscard]] TextWindowFontStyle final
	{
		std::optional<float> Size;
		std::optional<Snowing::BKDRHash> AnimationID;
		std::optional<Snowing::BKDRHash> ShaderID;
		std::optional<Snowing::Math::Vec3f> Color;

		TextWindowFontStyle& operator = (const TextWindowFontStyle&);

		static TextWindowFontStyle LoadFromProp(const Snowing::PropParser<char>& p);
		static TextWindowFontStyle LoadFromProp(const Snowing::Blob& p);
		static TextWindowFontStyle Combine(TextWindowFontStyle base, const TextWindowFontStyle& deriv);
		static TextWindowFontStyle Combine(const TextWindowFontStyle* styles, size_t count);
	};
}