#pragma once
#include <cassert>
#include <functional>
#include <utility>
#include "Object.h"
#include "MemPool.h"

namespace Snowing::Scene
{
	class[[nodiscard]] BaseTask : public Object
	{
	};

	template <typename TFunc>
	class Task : public BaseTask
	{
	private:
		TFunc func_;
		float time_;
		bool readyToCall_ = true;
	public:
		Task(float time, const TFunc& func) :
			time_{ time },
			func_{ func }
		{
			assert(time >= 0);
		}

		Task(float time,TFunc&& func):
			time_{ time },
			func_{ std::move(func) }
		{
			assert(time >= 0);
		}

		bool Update() override
		{
			time_ -= Engine::Get().DeltaTime();
			if (time_ <= 0)
			{
				if (readyToCall_)
				{
					readyToCall_ = false;
					func_();
				}
				return false;
			}
			else
				return true;
		}
	};

	class VirtualTask final : public Task<std::function<void()>>, public MemPool<VirtualTask>
	{
	public:
		using Task::Task;
	};

	class PointerTask final : public Task<void(*)()>, public MemPool<PointerTask>
	{
	public:
		using Task::Task;
	};
}