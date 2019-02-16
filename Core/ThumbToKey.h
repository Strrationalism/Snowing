#pragma once
#include <cassert>
#include <stdexcept>
#include "Controller.h"

namespace Snowing::Input
{
	template <typename Input,typename ThumbIDType>
	class[[nodiscard]] ThumbToKey final
	{
	private:
		const Input& input_;
		const ThumbIDType id_;
	public:
		ThumbToKey(const Input* input, ThumbIDType id) noexcept :
			input_{ *input },
			id_{ id }
		{
			assert(input);
		}

		[[nodiscard]]
		bool KeyPressed(ClassicGamepadArrow arrow) const
		{
			constexpr float filter = 0.95f;
			switch (arrow)
			{
			case ClassicGamepadArrow::Down:
				return input_.Thumb(id_).y > filter;
			case ClassicGamepadArrow::Up:
				return input_.Thumb(id_).y < -filter;
			case ClassicGamepadArrow::Left:
				return input_.Thumb(id_).x < -filter;
			case ClassicGamepadArrow::Right:
				return input_.Thumb(id_).x > filter;
			default:
				throw std::invalid_argument(__FUNCDNAME__ "Arrow ID not supported!");
			}
		}
	};
}
