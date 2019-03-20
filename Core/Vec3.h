#pragma once

namespace Snowing::Math
{
	template <typename T>
	struct[[nodiscard]] alignas(sizeof(T)) Vec3 final
	{
		T x, y, z;
	};

	using Vec3f = Vec3<float>;

	template <typename A, typename B>
	[[nodiscard]]
	constexpr bool operator == (const Vec3<A>& a, const Vec3<B>& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr bool operator != (const Vec3<A>& a, const Vec3<B>& b)
	{
		return !(a == b);
	}
}
