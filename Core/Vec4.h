#pragma once

namespace Snowing::Math
{
	template <typename T>
	struct[[nodiscard]] alignas(sizeof(T)) Vec4 final
	{
		T x, y, z, w;

		template <typename U>
		Vec4<U> Cast() const
		{
			return {
				static_cast<U>(x),
				static_cast<U>(y),
				static_cast<U>(z),
				static_cast<U>(w)
			};
		}
	};

	template <typename A,typename B>
	constexpr Vec4<A> operator - (const Vec4<A>& a, const Vec4<B>& b)
	{
		return {
			a.x - b.x,
			a.y - b.y,
			a.z - b.z,
			a.w - b.w
		};
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr bool operator == (const Vec4<A>& a, const Vec4<B>& b)
	{
		return
			a.x == b.x && a.y == b.y &&
			a.z == b.z && a.w == b.w;
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr bool operator != (const Vec4<A>& a, const Vec4<B>& b)
	{
		return
			a.x != b.x || a.y != b.y ||
			a.z != b.z || a.w != b.w;
	}


	using Vec4f = Vec4<float>;
}
