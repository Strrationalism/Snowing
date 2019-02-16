#pragma once
#include <memory>
#include <utility>

namespace Snowing
{
	template <typename T,typename ...TArgs>
	void Reinit(T& obj,TArgs&&... args)
	{
		std::destroy_at(&obj);
		new (&obj) T(std::forward<TArgs>(args)...);
	}

	template <typename T,typename ...TArgs,typename TFunc>
	void ReinitAfter(const TFunc& func,T& obj, TArgs&&... args)
	{
		std::destroy_at(&obj);
		func();
		new (&obj) T(std::forward<TArgs>(args)...);
	}
}