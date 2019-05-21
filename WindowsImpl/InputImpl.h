#pragma once
#include <Input.h>
#include "Handler.h"
#include "InputParam.h"
#include <queue>
#include "SingleInstance.h"

namespace Snowing::PlatformImpls::WindowsImpl
{
	class InputImpl final : public SingleInstance<InputImpl>
	{
	private:
		struct TouchPointInfo final
		{
			size_t orginalID;
			Snowing::Math::Vec2f pos;
		};

		const size_t touchInputCount_;
		std::vector<uint8_t> touchInputCache_;
		std::vector<TouchPointInfo> touchPoints_;
		std::queue<float> wheel_;
		bool windowFocused_ = false;
		std::optional<Math::Vec2f> mousePosition_;

		Input::InputInterface<PlatformImpls::WindowsImpl::InputImpl> keep_;
	public:
		InputImpl();
		InputImpl(const InputImpl&) = delete;
		InputImpl(InputImpl&&) = default;
		InputImpl& operator=(const InputImpl&) = delete;
		InputImpl& operator=(InputImpl&&) = default;

		void FocusWindow(bool b);

		bool PushMessage(const Handler pointerToWindowsMsg);

		bool KeyPressed(Input::MouseKey k) const;
		bool KeyPressed(Input::KeyboardKey k) const;
		bool KeyPressed(char k) const;
		bool KeyPressed(Input::TouchScreen::AnyPoint) const;
		bool KeyPressed(Input::TouchScreen) const = delete;

		std::optional<Math::Vec2f> Position(Input::MousePosition) const;
		std::optional<Math::Vec2f> Position(Input::TouchScreen::AnyPoint) const;
		std::optional<Math::Vec2f> Position(Input::TouchScreen) const = delete;
		size_t GetMaxTouchInputCount() const;

		float Trigger(Snowing::Input::MouseWheel) const;

		void Update();
	};
}

namespace Snowing::Input
{
	using Input = InputInterface<PlatformImpls::WindowsImpl::InputImpl>;
}
