#pragma once
#include "Object.h"
#include "MemPool.h"
#include <functional>

namespace Snowing::Scene
{
	template <typename TFunc>
	class FrameFunc : public Object
	{
	private:
		TFunc func_;
	public:
		template <typename TFunc>
		FrameFunc(TFunc&& func):
			func_{std::forward<TFunc>(func)}
		{}

		bool Update() override
		{
			return func_();
		}
	};

	class VirtualFrameFunc final : public FrameFunc<std::function<bool()>>, public MemPool<VirtualFrameFunc>
	{
	public:
		using FrameFunc::FrameFunc;
	};

	class PointerFrameFunc final : public FrameFunc<bool(*)()>, public MemPool<VirtualFrameFunc>
	{
	public:
		using FrameFunc::FrameFunc;
	};
}
