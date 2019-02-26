#pragma once

namespace Snowing
{
	class NoCopyMove
	{
	public:
		NoCopyMove() = default;
		virtual ~NoCopyMove() = default;
		NoCopyMove(const NoCopyMove&) = delete;
		NoCopyMove& operator=(const NoCopyMove&) = delete;
		NoCopyMove(NoCopyMove&&) = delete;
	};
}