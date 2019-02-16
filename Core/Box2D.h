#pragma once
#include "Vec2.h"
#include "Vec4.h"
#include "SMath.h"

namespace Snowing::Math
{
	template <typename A,typename B>
	constexpr bool IsPositionInBox(const Math::Vec2<A>& pos, const Math::Vec4<B>& box)
	{
		return
			IsBetween(pos.x, box.x, box.x + box.z) &&
			IsBetween(pos.y, box.y, box.y + box.w);
	}
}
