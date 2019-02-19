#pragma once
#include "Vec2.h"

namespace Snowing::Math
{
	struct[[nodiscard]] alignas(4) Coordinate2DCenter final
	{
		Math::Vec2f Center;
		Math::Vec2f Space;
	};

	struct[[nodiscard]] alignas(4) Coordinate2DRect final
	{
		Math::Vec2f LeftTop;
		Math::Vec2f RightBottom;
	};

	[[nodiscard]]
	constexpr Coordinate2DCenter ConvertCoordnate2DToCenter(const Coordinate2DRect& r)
	{
		return
		{
			(r.RightBottom + r.LeftTop) / 2,
			r.RightBottom - r.LeftTop
		};
	}

	[[nodiscard]]
	constexpr Coordinate2DCenter ConvertCoordnate2DToCenter(const Coordinate2DCenter& r)
	{
		return r;
	}

	[[nodiscard]]
	constexpr Coordinate2DRect ConvertCoordnate2DToRect(const Coordinate2DRect& r)
	{
		return r;
	}

	template <typename T,typename CoordA,typename CoordB>
	[[nodiscard]]
	constexpr inline auto ConvertPosition2DCoordinate(
		const Math::Vec2<T>& pos,
		const CoordA& src,
		const CoordB& dst)
	{
		auto srcc = ConvertCoordnate2DToCenter(src);
		auto dstc = ConvertCoordnate2DToCenter(dst);
		const auto t = (pos - srcc.Center) / srcc.Space;
		return t * dstc.Space + dstc.Center;
	}
}
