#pragma once

namespace Snowing::Math
{
	template <typename T>
	struct[[nodiscard]] alignas(sizeof(T)) Vec3 final
	{
		T x, y, z;
	};

	using Vec3f = Vec3<float>;
}
