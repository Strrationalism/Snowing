#pragma once
#include "Task.h"

namespace Snowing::Scene
{
	template <typename TCondition,typename TFunc>
	class [[nodiscard]] ConditionTask final : public BaseTask
	{
	private:
		TCondition condition_;
		TFunc func_;

		bool finished_ = false;

	public:
		template <typename TCondition, typename TFunc>
		ConditionTask(TCondition&& condition,TFunc&& func):
			condition_{ std::forward<TCondition>(condition) },
			func_{ std::forward<TFunc>(func) }
		{}

		bool Update() override
		{
			if (!finished_)
			{
				if (condition_())
				{
					func_();
					finished_ = true;
					return false;
				}
				else
					return true;
			}
			else
				return false;
		}
	};

	using VirtualConditionTask = ConditionTask<std::function<bool()>, std::function<void()>>;
}
