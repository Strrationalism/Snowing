#pragma once
#include <cassert>
#include "InputParam.h"
#include "Input.h"

namespace Snowing::Input
{
	class[[nodiscard]] KeyWatcherBase
	{
	public:
		virtual ~KeyWatcherBase() = default;

		virtual void Update() = 0;
		virtual bool JustPress() const noexcept = 0;
		virtual bool JustRelease() const noexcept = 0;
		virtual bool KeyPressed() const noexcept = 0;
	};

	template <typename Input,typename ...KeyIDType>
	class[[nodiscard]] KeyWatcher final : public KeyWatcherBase
	{
	private:
		const std::tuple<KeyIDType...> keyID_;
		const Input& input_;
		bool prevState_ = false, curState_ = false;

	public:
		KeyWatcher(const Input* input,KeyIDType... keyID) noexcept :
			input_{ *input },
			keyID_{ keyID... }
		{
			assert(input);
		}

		void Update() override
		{
			const auto p = [this](KeyIDType... args) {return input_.KeyPressed(args...); };

			prevState_ = curState_;
			curState_ = std::apply(p, keyID_);
		}

		[[nodiscard]]
		bool JustPress() const noexcept override
		{
			return curState_ && !prevState_;
		}

		[[nodiscard]]
		bool JustRelease() const noexcept override
		{
			return !curState_ && prevState_;
		}

		[[nodiscard]]
		bool KeyPressed() const noexcept override
		{
			return curState_;
		}
	};
}
