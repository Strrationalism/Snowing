#pragma once
#include <optional>
#include "Group.h"
#include "MemPool.h"

namespace Snowing::Scene
{
	template <typename TMenuItem>
	class[[nodiscard]] Menu final : public Group<TMenuItem>,public MemPool<Menu<TMenuItem>>
	{
	private:
		std::optional<size_t> selected_;

		struct SelectTrait
		{
			using Test1 = decltype(std::declval<TMenuItem>().OnSelected());
			using Test2 = decltype(std::declval<TMenuItem>().OnUnselected());
			static_assert(std::is_void<Test1>::value);
			static_assert(std::is_void<Test2>::value);
		};
		constexpr static SelectTrait trait_{};

	public:
		std::optional<size_t> GetSelectedIndex() const
		{
			return selected_;
		}

		std::optional<TMenuItem*> GetSelectedObject() const
		{
			if (selected_.has_value())
				return (*this)[selected_.value()];
			else
				return std::nullopt;
		}

		void Unselect()
		{
			auto currentMenuItem = GetSelectedObject();
			if (currentMenuItem.has_value())
				currentMenuItem.value()->OnUnselected();
			selected_.reset();
		}

		void Select(size_t index)
		{
			Unselect();
			selected_ = index;
			(*this)[index]->OnSelected();
		}
	};
}
