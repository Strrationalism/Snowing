#pragma once
#include <future>
#include "Utils.h"
#include "Object.h"

namespace Snowing::Scene
{
	template <typename TObject,typename TGroupType = Group<>>
	class AsyncLauncher final : public Object
	{
	private:
		std::future<std::unique_ptr<TObject>> asyncObject_;
		TGroupType* targetGroup_;

		bool finished_ = false;
		bool canceled_ = false;
	public:

		template <typename ... TArgs>
		AsyncLauncher(TGroupType* targetGroup,TArgs... args):
			asyncObject_{ 
				std::async(
					std::launch::async,
					[](TArgs ... args) {return std::make_unique<TObject>(args...); },
					std::move(args)...)
			},
			targetGroup_{ targetGroup }
		{}

		bool Update() override
		{
			if (!finished_)
			{
				if (Snowing::FutureReady(asyncObject_))
				{
					if(!canceled_)
						targetGroup_->PushBack(asyncObject_.get());
					finished_ = true;
				}
			}

			return !finished_;
		}

		void Cancel()
		{
			canceled_ = true;
		}
	};
}
