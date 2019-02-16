#pragma once
#include <type_traits>
#include <new>
#include <queue>

namespace Snowing
{
	template <typename PooledClass>
	class MemPool
	{
	private:
		static inline std::queue<std::unique_ptr<uint8_t[]>> freeList_;

	public:
		void * operator new (size_t size, const std::nothrow_t&) noexcept
		{
			static_assert(std::is_final<PooledClass>::value, "PooledClass must is a final class.");
			constexpr static size_t S = sizeof(PooledClass);
			assert(size <= S);

			if (freeList_.empty())
				return ::operator new(S, std::nothrow_t{});
			else
			{
				auto& u = freeList_.front();
				const auto p = reinterpret_cast<void*>(u.get());
				u.release();
				freeList_.pop();
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
			freeList_.push(std::unique_ptr<uint8_t[]>{ reinterpret_cast<uint8_t*>(p) });
		}
	};
}