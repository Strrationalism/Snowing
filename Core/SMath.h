#pragma once

namespace Snowing::Math::Constants
{
	template <typename T>
	constexpr T E = static_cast<T>(2.71828182845904523536);

	template <typename T>
	constexpr T Log2E = static_cast<T>(1.44269504088896340736);

	template <typename T>
	constexpr T Log10E = static_cast<T>(0.434294481903251827651);

	template <typename T>
	constexpr T Ln2 = static_cast<T>(0.693147180559945309417);

	template <typename T>
	constexpr T Ln10 = static_cast<T>(2.30258509299404568402);

	template <typename T>
	constexpr T Pi = static_cast<T>(3.14159265358979323846);

	template <typename T>
	constexpr T Sqrt2 = static_cast<T>(1.41421356237309504880);
}

namespace Snowing::Math
{
	template <typename T,typename A,typename B>
	constexpr auto Interpo(const T& t,const A& a, const B& b)
	{
		return (b - a) * t + a;
	}

	template <typename T>
	constexpr auto IsBetween(const T& v, const T& a, const T& b)
	{
		return (v >= a && v <= b) || (v >= b && v <= a);
	}
}
