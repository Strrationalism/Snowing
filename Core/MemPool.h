#pragma once
#include <type_traits>
#include <new>
#include <queue>
#include "NoCopyMove.h"

namespace Snowing
{
	template <typename PooledClass>
	class MemPool
	{
	private:

		struct FreeList : NoCopyMove
		{
			std::queue<void*> freeList_;
			size_t allocCount_ = 0;

			~FreeList()
			{
				while (!freeList_.empty())
				{
					free(freeList_.front());
					freeList_.pop();
				}
			}
		};

		static inline FreeList freeList_;

	public:
		void * operator new (size_t size, const std::nothrow_t&) noexcept
		{
			static_assert(std::is_final<PooledClass>::value, "PooledClass must is a final class.");
			constexpr static size_t S = sizeof(PooledClass);
  			assert(size == S);

			freeList_.allocCount_++;

			if (freeList_.freeList_.empty())
				return malloc(size);
			else
			{
				auto p = freeList_.freeList_.front();
				freeList_.freeList_.pop();
				return p;
			}
		}

		void * operator new (size_t size)
		{
			auto p = operator new(size, std::nothrow_t{});
			if (p)
				return p;
			else
				throw std::bad_alloc{};
		}

		void operator delete (void *p)
		{
			freeList_.allocCount_--;
			freeList_.freeList_.push(p);
		}
	};
}