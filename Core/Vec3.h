#pragma once

namespace Snowing::Math
{
	template <typename T>
	struct[[nodiscard]] alignas(sizeof(T)) Vec3 final
	{
		T x, y, z;

		template <typename U>
		Vec3<U> Cast() const
		{
			return {
				static_cast<U>(x),
				static_cast<U>(y),
				static_cast<U>(z)
			};
		}
	};

	using Vec3f = Vec3<float>;

	template <typename A, typename B>
	constexpr Vec3<A> operator + (const Vec3<A>& a, const Vec3<B>& b)
	{
		return {
			a.x + b.x,
			a.y + b.y,
			a.z + b.z
		};
	}

	template <typename A, typename B>
	constexpr Vec3<A> operator - (const Vec3<A>& a, const Vec3<B>& b)
	{
		return {
			a.x - b.x,
			a.y - b.y,
			a.z - b.z
		};
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec3<A> operator * (const Vec3<A>& a, const B& b) noexcept
	{
		return
		{
			a.x * b,
			a.y * b,
			a.z * b
		};
	}

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
