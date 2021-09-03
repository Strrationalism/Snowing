#pragma once
#include <cassert>
#include <memory>
#include <utility>
#include <future>

namespace Snowing
{
	template <typename T,typename ...TArgs>
	void Reinit(T& obj,TArgs&&... args)
	{
		std::destroy_at(&obj);
		new (&obj) T(std::forward<TArgs>(args)...);
	}

	template <typename TFuture>
	bool FutureReady(const TFuture& future)
	{
		using namespace std::chrono;

		if (future.valid())
		{
			const auto status = future.wait_for(0s);
			return status == std::future_status::ready;
		}
		else
			return false;
	}
}