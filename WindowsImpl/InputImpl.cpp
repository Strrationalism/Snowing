#include "stdafx.h"
#include "InputImpl.h"


using namespace Snowing::Input;
using namespace Snowing::PlatformImpls::WindowsImpl;


void Snowing::PlatformImpls::WindowsImpl::InputImpl::FocusWindow(bool b)
{
	windowFocused_ = b;
}

bool Snowing::PlatformImpls::WindowsImpl::InputImpl::PushMessage(const Handler pointerToWindowsMsg)
{
	const MSG* msg = pointerToWindowsMsg.Get<MSG*>();
	switch (msg->message)
	{
	
	case WM_MOUSEWHEEL:
	{
		const auto delta = static_cast<short>(HIWORD(msg->wParam));
		wheel_.push(0.0f);
		wheel_.push(delta > 0 ? -1.0f : 1.0f);
		break;
	}

	case WM_MOUSEMOVE:
	{
		RECT rect;
		GetClientRect(msg->hwnd, &rect);
		const Math::Vec2<float> size
		{
			static_cast<float>(rect.right - rect.left),
			static_cast<float>(rect.bottom - rect.top)
		};
		const Math::Vec2f pos
		{
			LOWORD(msg->lParam) / size.x,
			HIWORD(msg->lParam) / size.y
		};

		mousePosition_ = pos;

		TRACKMOUSEEVENT tme
		{
			sizeof(tme),
			TME_LEAVE,
			msg->hwnd,
			HOVER_DEFAULT
		};
		TrackMouseEvent(&tme);

		break;
	}

	case WM_MOUSELEAVE:
		mousePosition_.reset();
		break;

	default:
		return false;
		break;
	};
	return true;
}

#undef TranslateSimpleMsg
#undef TranslateXButton


static bool keyState(int vk,bool wf)
{
	if (!wf)
		return false;
	else
		return (GetAsyncKeyState(vk) & 0x8001) > 0;
}

#define MAPKEY(SnowingKey,VK) \
case Snowing::Input::SnowingKey:\
	return keyState(VK,windowFocused_)

bool Snowing::PlatformImpls::WindowsImpl::InputImpl::KeyPressed(Input::MouseKey k) const
{
	switch (k)
	{
		MAPKEY(MouseKey::Left, VK_LBUTTON);
		MAPKEY(MouseKey::Middle, VK_MBUTTON);
		MAPKEY(MouseKey::Right, VK_RBUTTON);
		MAPKEY(MouseKey::SideKeyA, VK_XBUTTON1);
		MAPKEY(MouseKey::SideKeyB, VK_XBUTTON2);
	default:
		throw std::invalid_argument(__FUNCDNAME__ "Not Supported Mouse Key");
	}
}


bool Snowing::PlatformImpls::WindowsImpl::InputImpl::KeyPressed(Snowing::Input::KeyboardKey k) const
{
	switch (k)
	{
		MAPKEY(KeyboardKey::Escape, VK_ESCAPE);
		MAPKEY(KeyboardKey::Enter, VK_RETURN);
		MAPKEY(KeyboardKey::Space, VK_SPACE);
		MAPKEY(KeyboardKey::Up, VK_UP);
		MAPKEY(KeyboardKey::Down, VK_DOWN);
		MAPKEY(KeyboardKey::Left, VK_LEFT);
		MAPKEY(KeyboardKey::Right, VK_RIGHT);
		MAPKEY(KeyboardKey::LShift, VK_LSHIFT);
		MAPKEY(KeyboardKey::RShift, VK_RSHIFT);
		MAPKEY(KeyboardKey::LCtrl, VK_LCONTROL);
		MAPKEY(KeyboardKey::RCtrl, VK_RCONTROL);
		MAPKEY(KeyboardKey::F1, VK_F1);
		MAPKEY(KeyboardKey::F2, VK_F2);
		MAPKEY(KeyboardKey::F3, VK_F3);

	default:
		throw std::invalid_argument(__FUNCDNAME__ "Not Supported Keyboard Key");
	}
}

bool Snowing::PlatformImpls::WindowsImpl::InputImpl::KeyPressed(char k) const
{
	if((k >= 'A' && k <= 'Z') || (k >= '0' && k <= '9')) return keyState(k, windowFocused_);
	else throw std::invalid_argument(__FUNCDNAME__ "Not Supported Charater Keyboard Key");
}
std::optional<Snowing::Math::Vec2f> Snowing::PlatformImpls::WindowsImpl::InputImpl::Position(Input::MousePosition) const
{
	return mousePosition_;
}
float Snowing::PlatformImpls::WindowsImpl::InputImpl::Trigger(Snowing::Input::MouseWheel) const
{
	if (wheel_.empty())
		return 0.0f;
	else
		return wheel_.front();
}
void Snowing::PlatformImpls::WindowsImpl::InputImpl::Update()
{
	if (!wheel_.empty())
		wheel_.pop();
}
#undef MAPKEY

