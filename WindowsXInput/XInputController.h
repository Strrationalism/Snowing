#pragma once
#include <cstdint>
#include <DataBox.h>
#include <vector>
#include "Vec2.h"
#include "Controller.h"

#pragma comment(lib,"Xinput9_1_0.lib")

namespace Snowing::PlatformImpls::WindowsXInput
{
	class XInputControllerImpl final
	{
	private:
		constexpr static size_t XInputStateSize = 16;

		WindowsImpl::DataBox<XInputStateSize> state_;
		const std::uintptr_t userID_;
	public:
		XInputControllerImpl(std::uintptr_t userID);

		bool Update();
		bool KeyPressed(Input::ClassicGamepadKeys k) const;
		bool KeyPressed(Input::ClassicGamepadArrow k) const;
		Math::Vec2f Thumb(Input::ClassicGamepadThumbs a) const;
		float Trigger(Input::ClassicGamepadTriggers t) const;
		void Vibration(Input::VibrationLeftRight) const;
		void Vibration(Input::VibrationStop) const;
	};

	using XInputController = Snowing::Input::ControllerInterface<XInputControllerImpl>;

	[[nodiscard]]
	std::vector<XInputController> GetConnectedControllers();
}
