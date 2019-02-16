#pragma once

namespace Snowing::Math
{
	template <typename T>
	struct[[nodiscard]] alignas(sizeof(T)) Vec2 final
	{
		T x, y;
	};

	template <typename A, typename B>
	[[nodiscard]]
	constexpr bool operator == (const Vec2<A>& a, const Vec2<B>& b)
	{
		return a.x == b.x && a.y == b.y;
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr bool operator != (const Vec2<A>& a, const Vec2<B>& b)
	{
		return a.x != b.x || a.y != b.y;
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec2<A> operator * (const Vec2<A>& a, const B& b) noexcept
	{
		return
		{
			a.x * b,
			a.y * b
		};
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec2<A> operator / (const Vec2<A>& a, const B& b) noexcept
	{
		return
		{
			a.x / b,
			a.y / b
		};
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec2<A> operator * (const Vec2<A>& a, const Vec2<B>& b) noexcept
	{
		return
		{
			a.x * b.x,
			a.y * b.y
		};
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec2<A> operator / (const Vec2<A>& a, const Vec2<B>& b) noexcept
	{
		return
		{
			a.x / b.x,
			a.y / b.y
		};
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec2<A> operator + (const Vec2<A>& a, const Vec2<B>& b) noexcept
	{
		return
		{
			a.x + b.x,
			a.y + b.y
		};
	}

	template <typename T>
	[[nodiscard]]
	constexpr Vec2<T> operator - (const Vec2<T>& a) noexcept
	{
		return { -a.x,-a.y };
	}

	template <typename A, typename B>
	[[nodiscard]]
	constexpr Vec2<A> operator - (const Vec2<A>& a, const Vec2<B>& b) noexcept
	{
		return a + (-b);
	}

	template <typename A, typename B>
	constexpr Vec2<A>& operator -= (Vec2<A>& a, const Vec2<B>& b) noexcept
	{
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	using Vec2f = Vec2<float>;
}
