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
		ConditionTask(const TCondition& condition,const TFunc& func):
			condition_{ condition },
			func_{ func }
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
}
