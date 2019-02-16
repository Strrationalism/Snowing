#include "XInputController.h"
#include <Windows.h>
#include <Xinput.h>
#include <cassert>


using namespace Snowing;
using namespace Snowing::PlatformImpls::WindowsXInput;

Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::XInputControllerImpl(std::uintptr_t userID):
	userID_{ userID },
	state_{ XINPUT_STATE{ 0 } }
{
}


bool Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::Update()
{
	static_assert(sizeof(XINPUT_STATE) == XInputStateSize);
	return XInputGetState((DWORD)userID_, &state_.GetWrite<XINPUT_STATE>()) == ERROR_SUCCESS;
}

#define Btn(From,To) \
case Input::ClassicGamepadKeys::From: \
	return (state_.Get<XINPUT_STATE>().Gamepad.wButtons & To) != 0

bool Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::KeyPressed(Input::ClassicGamepadKeys k) const
{
	switch (k)
	{
		Btn(A, XINPUT_GAMEPAD_A);
		Btn(B, XINPUT_GAMEPAD_B);
		Btn(X, XINPUT_GAMEPAD_X);
		Btn(Y, XINPUT_GAMEPAD_Y);
		Btn(L, XINPUT_GAMEPAD_LEFT_SHOULDER);
		Btn(R, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		Btn(LThumb, XINPUT_GAMEPAD_LEFT_THUMB);
		Btn(RThumb, XINPUT_GAMEPAD_RIGHT_THUMB);
		Btn(Start, XINPUT_GAMEPAD_START);
		Btn(SelectOrBack, XINPUT_GAMEPAD_BACK);

	default:
		throw std::invalid_argument(__FUNCDNAME__ ":No Key Supported!");
	}
}

#undef Btn
#define Btn(From,To) \
case Input::ClassicGamepadArrow::From: \
	return (state_.Get<XINPUT_STATE>().Gamepad.wButtons & To) != 0

bool Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::KeyPressed(Input::ClassicGamepadArrow k) const
{
	switch (k)
	{
		Btn(Up, XINPUT_GAMEPAD_DPAD_UP);
		Btn(Down, XINPUT_GAMEPAD_DPAD_DOWN);
		Btn(Left, XINPUT_GAMEPAD_DPAD_LEFT);
		Btn(Right, XINPUT_GAMEPAD_DPAD_RIGHT);

	default:
		throw std::invalid_argument(__FUNCDNAME__ ":No Arrow Supported!");
	};
}


Math::Vec2f Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::Thumb(Input::ClassicGamepadThumbs a) const
{
	SHORT x,y;
	const auto& gp = state_.Get<XINPUT_STATE>().Gamepad;
	switch (a)
	{
	case Input::ClassicGamepadThumbs::LThumb:
		x = gp.sThumbLX;
		y = gp.sThumbLY;
		break;
	case Input::ClassicGamepadThumbs::RThumb:
		x = gp.sThumbRX;
		y = gp.sThumbRY;
		break;
	default:
		throw std::invalid_argument(__FUNCDNAME__ ":No Thumb Supported!");
	}

	auto trans = [](SHORT i)
	{
		if (i == -32768) i = -32767;
		return i / 32767.0f;
	};

	return 
	{
		trans(x),
		-trans(y)
	};
}


float Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::Trigger(Input::ClassicGamepadTriggers t) const
{
	BYTE v;
	const auto& gp = state_.Get<XINPUT_STATE>().Gamepad;
	switch (t)
	{
	case Snowing::Input::ClassicGamepadTriggers::LTrigger:
		v = gp.bLeftTrigger;
		break;
	case Snowing::Input::ClassicGamepadTriggers::RTrigger:
		v = gp.bRightTrigger;
		break;
	default:
		throw std::invalid_argument(__FUNCDNAME__ ":No Trigger Supported!");
	}

	return v / 255.0f;
}

void Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::Vibration(Input::VibrationLeftRight p) const
{
	assert(p.l >= 0.0f && p.l <= 1.0f);
	assert(p.r >= 0.0f && p.r <= 1.0f);
	XINPUT_VIBRATION vib
	{
		static_cast<WORD>(p.l * 65535),
		static_cast<WORD>(p.r * 65535)
	};

	XInputSetState((DWORD)userID_, &vib);
}

void Snowing::PlatformImpls::WindowsXInput::XInputControllerImpl::Vibration(Input::VibrationStop) const
{
	XINPUT_VIBRATION vib
	{
		static_cast<WORD>(0),
		static_cast<WORD>(0)
	};

	XInputSetState((DWORD)userID_, &vib);
}

std::vector<XInputController> Snowing::PlatformImpls::WindowsXInput::GetConnectedControllers()
{
	std::vector<XInputController> r;
	for (std::uintptr_t i = 0u; i < static_cast<std::uintptr_t>(XUSER_MAX_COUNT); ++i)
	{
		XINPUT_STATE state;
		if (XInputGetState((DWORD)i, &state) == ERROR_SUCCESS)
			r.emplace_back(XInputControllerImpl {i});
	}
	return r;
}
