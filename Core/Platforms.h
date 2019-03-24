#pragma once
#include <type_traits>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>
#include <array>
#include <cassert>
#include <functional>

namespace Snowing::Platforms
{
	enum class EngineImplName
	{
		Windows
	};

	template <template <typename...> class TInterface, typename... TInterfaceImpl>
	struct IsInterface
	{
		constexpr static bool Value = false;
	};

	template <template <typename...> class TInterface, typename... TImpl>
	struct IsInterface<TInterface, TInterface<TImpl...>>
	{
		constexpr static bool Value = true;
	};

	template <template <typename...> class TInterface, typename TImpl>
	void AssertInterface(const TImpl& impl)
	{
		static_assert(IsInterface<TInterface, TImpl>::Value, "Interface Assert Fault.");
	}

	class Handler final
	{
	public:
		using Deleter = std::function<void(void*)>;
		static inline void DoNothingDeleter(void *) {}
	private:
		void* ptr_;
		Deleter deleter_;

#ifdef _DEBUG
		std::type_index type_;
#endif // _DEBUG

	public:
		inline Handler() noexcept:
			ptr_{ nullptr },
			deleter_{ nullptr }
#ifdef _DEBUG
			,type_{ typeid(this) }
#endif // _DEBUG	
		{
		}

		template<typename T>
		inline Handler(T ptr, Deleter deleter) :
			ptr_{ ptr },
			deleter_{ deleter }
#ifdef _DEBUG
			, type_{ typeid(T) }
#endif
		{
			assert(ptr);
		}

		Handler(const Handler&) = delete;
		inline Handler(Handler&& a) noexcept:
			ptr_{ a.ptr_ },
			deleter_{ a.deleter_ }
#ifdef _DEBUG
			, type_{ std::move(a.type_) }
#endif // _DEBUG	
		{
			a.ptr_ = nullptr;
		}


		Handler& operator=(const Handler&) = delete;
		inline Handler& operator=(Handler&& a) noexcept
		{
			if (ptr_)
				deleter_(ptr_);

			ptr_ = a.ptr_;
			deleter_ = a.deleter_;

#ifdef _DEBUG
			type_ = a.type_;
#endif // _DEBUG
			a.ptr_ = nullptr;
			return *this;
		}


		inline ~Handler()
		{
			if (ptr_)
				deleter_(ptr_);
		}

		template<typename T>
		T Get() const
		{
			assert(type_ == typeid(T));
			assert(ptr_);
			return static_cast<T>(ptr_);
		}

		template <typename TGet, typename TCastTo>
		TCastTo Cast() const
		{
			return static_cast<TCastTo>(Get<TGet>());
		}

		inline bool IsSome() const noexcept
		{
			return ptr_ != nullptr;
		}

		void Clear()
		{
			if (ptr_)
				deleter_(ptr_);
			ptr_ = nullptr;
		}
	};

	template <size_t Size>
	class[[nodiscard]] DataBox final
	{
	private:
		std::array<std::uint8_t, Size> data_;

#ifdef _DEBUG
		std::type_index type_;
#endif // _DEBUG

	public:

		constexpr static auto DataSize = Size;

		template <typename T>
		DataBox(const T& data) noexcept
#ifdef _DEBUG
			:type_{ typeid(T) }
#endif // _DEBUG
		{
			static_assert(std::is_pod<T>::value,"DataBox must box the pod type.");
			static_assert(sizeof(T) == Size,"Data size not matched.");

			const auto bytes = reinterpret_cast<const std::uint8_t*>(&data);
			std::copy(bytes, bytes + Size, data_.begin());
		}

		template <typename T>
		const T& Get() const
		{
			assert(type_ == typeid(T));
			return *reinterpret_cast<const T*>(data_.data());
		}

		template <typename T>
		T& GetWrite()
		{
			assert(type_ == typeid(T));
			return *reinterpret_cast<T*>(data_.data());
		}
	};
}

