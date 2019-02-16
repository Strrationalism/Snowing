#pragma once
#include <cassert>

namespace Snowing
{
	template <typename SingleInstanceClass>
	class SingleInstance
	{
	private:
		static inline SingleInstanceClass *current_ = nullptr;
	
	protected:
		SingleInstance() noexcept
		{
			assert(current_ == nullptr);
			current_ = static_cast<SingleInstanceClass*>(this);
		}

		SingleInstance(SingleInstance&& a) noexcept
		{
			current_ = static_cast<SingleInstanceClass*>(this);
		}

		SingleInstance(const SingleInstance&) = delete;
		SingleInstance& operator = (const SingleInstance&) = delete;
		SingleInstance& operator = (SingleInstance&&) = delete;

		~SingleInstance()
		{
			if(current_ == static_cast<SingleInstanceClass*>(this))
				current_ = nullptr;
		}

	public:
		static SingleInstanceClass& Get()
		{
			assert(current_);
			return *current_;
		}
	};
}
