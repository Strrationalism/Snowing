#include "pch.h"
#include "AVGInput.h"
#include <Snowing.h>

using namespace Snowing::Input;

bool Yukimi::AVGInput::AVGInputKeyboardMouse::KeyPressed(AVGKeys k) const
{
	using Input = Snowing::Input::Input;
	switch (k)
	{
	case AVGKeys::Ok:
		if (Input::KeyPressed(KeyboardKey::Enter)) return true;
		break;

	case AVGKeys::Cancel:
		if (Input::KeyPressed(KeyboardKey::Escape)) return true;
		if (Input::KeyPressed(MouseKey::Right)) return true;
		if (Input::KeyPressed(MouseKey::SideKeyB)) return true;
		break;
	case AVGKeys::Click:
		if (Input::KeyPressed(MouseKey::Left)) return true;
		break;
	case AVGKeys::HideTextWindow:
		if (Input::KeyPressed(MouseKey::Middle)) return true;
		if (Input::KeyPressed(KeyboardKey::RShift)) return true;
		if (Input::KeyPressed(KeyboardKey::LShift)) return true;
		break;
	case AVGKeys::Skip:
		if (Input::KeyPressed(KeyboardKey::LCtrl)) return true;
		if (Input::KeyPressed(KeyboardKey::RCtrl)) return true;
		break;
	case AVGKeys::Space:
		if (Input::KeyPressed(KeyboardKey::Space)) return true;
		break;
	default:
		assert(false);
	}
	return false;
}

bool Yukimi::AVGInput::AVGInputKeyboardMouse::KeyPressed(Snowing::Input::ClassicGamepadArrow a) const
{
	using Input = Snowing::Input::Input;
	switch (a)
	{
	case Snowing::Input::ClassicGamepadArrow::Up:
		if (Input::KeyPressed(KeyboardKey::Up)) return true;
		if (Input::Trigger(MouseWheel{}) < 0) return true;
		break;
	case Snowing::Input::ClassicGamepadArrow::Down:
		if (Input::KeyPressed(KeyboardKey::Down)) return true;
		if (Input::Trigger(MouseWheel{}) > 0) return true;
		break;
	case Snowing::Input::ClassicGamepadArrow::Left:
		if (Input::KeyPressed(KeyboardKey::Left)) return true;
		break;
	case Snowing::Input::ClassicGamepadArrow::Right:
		if (Input::KeyPressed(KeyboardKey::Right)) return true;
		break;
	default:
		break;
	}

	return false;
}

void Yukimi::AVGInput::AVGInputKeyboardMouse::Vibration(Snowing::Input::VibrationLeftRight)
{
}

void Yukimi::AVGInput::AVGInputKeyboardMouse::Vibration(Snowing::Input::VibrationStop)
{
}

Yukimi::AVGInput::AVGInput()
{
	Emplace<AVGInputKeyboardMouse>();
}

bool Yukimi::AVGInput::KeyPressed(AVGKeys a) const
{
	return std::any_of(devices_.begin(), devices_.end(), [a](auto & p) {
		return p->KeyPressed(a);
	});
}

bool Yukimi::AVGInput::KeyPressed(Snowing::Input::ClassicGamepadArrow a) const
{
	return std::any_of(devices_.begin(), devices_.end(), [a](auto & p) {
		return p->KeyPressed(a);
	});
}

void Yukimi::AVGInput::Vibration(Snowing::Input::VibrationLeftRight s)
{
	for (auto& p : devices_)
		p->Vibration(s);
}

void Yukimi::AVGInput::Vibration(Snowing::Input::VibrationStop s)
{
	for (auto& p : devices_)
		p->Vibration(s);
}

std::optional<Snowing::Math::Vec2f> Yukimi::AVGInput::Position(Snowing::Input::MousePosition m)
{
	return Snowing::Input::Input::Position(m);
}
