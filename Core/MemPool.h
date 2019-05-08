#pragma once
#include <type_traits>
#include <new>
#include <queue>
#include "NoCopyMove.h"

namespace Snowing
{
#ifdef _DEBUG
	template <size_t Size>
	class SizedMemPool
	{
	public:
		static void* New(size_t size, const std::nothrow_t& t) noexcept
		{
			return ::operator new(size, t);
		}

		static void* New(size_t size)
		{
			return ::operator new(size);
		}

		static void Delete(void* p)
		{
			return ::operator delete(size);
		}
	};

#else

	template <size_t Size>
	class SizedMemPool final
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
		SizedMemPool() = delete;
		SizedMemPool(const SizedMemPool&) = delete;
		SizedMemPool(SizedMemPool&&) = delete;

		static void * New(size_t size, const std::nothrow_t&) noexcept
		{
			constexpr static size_t S = Size;
			if (size != S)
				throw std::exception{ "Unknown mempool error." };

			freeList_.allocCount_++;

			if (freeList_.freeList_.empty())
				return malloc(S);
			else
			{
				auto p = freeList_.freeList_.back();
				freeList_.freeList_.pop_back();
				return p;
			}
		}

		static void * New(size_t size)
		{
			auto p = operator new(size, std::nothrow_t{});
			if (p)
				return p;
			else
				throw std::bad_alloc{};
		}

		static void Delete(void *p)
		{
			freeList_.allocCount_--;
			freeList_.freeList_.push_back(p);
		}
	};
#endif // _DEBUG

	template <typename T>
	class MemPool 
	{
	public:
		void* operator new(size_t size, const std::nothrow_t& t) noexcept
		{
			static_assert(std::is_final<T>::value, "T must is a final class.");
			return SizedMemPool<sizeof(T)>::New(size, t);
		}

		void* operator new(size_t size) noexcept
		{
			static_assert(std::is_final<T>::value, "T must is a final class.");
			return SizedMemPool<sizeof(T)>::New(size);
		}

		void operator delete(void* p)
		{
			static_assert(std::is_final<T>::value, "T must is a final class.");
			return SizedMemPool<sizeof(T)>::Delete(p);
		}
		
	};

}