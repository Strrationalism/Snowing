#pragma once
#include <type_traits>
#include <new>
#include <queue>
#include "NoCopyMove.h"

namespace Snowing
{
#ifdef _DEBUG
	template <typename T>
	class MemPool
	{};
#else

	template <typename PooledClass>
	class MemPool
	{
	private:

		struct FreeList : NoCopyMove
		{
			std::vector<void*> freeList_;
			size_t allocCount_ = 0;

			~FreeList()
			{
				while (!freeList_.empty())
				{
					free(freeList_.back());
					freeList_.pop_back();
				}
				freeList_.shrink_to_fit();
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
				auto p = freeList_.freeList_.back();
				freeList_.freeList_.pop_back();
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
			freeList_.freeList_.push_back(p);
		}
	};
#endif // _DEBUG

}