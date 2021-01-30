#include "stdafx.h"
#include "InputImpl.h"
#include "Box2D.h"
#include "WindowImpl.h"

using namespace Snowing::Input;
using namespace Snowing::PlatformImpls::WindowsImpl;


Snowing::PlatformImpls::WindowsImpl::InputImpl::InputImpl():
	touchInputCount_{ static_cast<size_t>(GetSystemMetrics(SM_MAXIMUMTOUCHES)) }
{
	static_assert(std::is_pod<TOUCHINPUT>::value);
	touchInputCache_.reserve(sizeof(TOUCHINPUT) * touchInputCount_);
	touchPoints_.reserve(touchInputCount_);
}

void Snowing::PlatformImpls::WindowsImpl::InputImpl::FocusWindow(bool b)
{
	windowFocused_ = b;
}

bool Snowing::PlatformImpls::WindowsImpl::InputImpl::PushMessage(const Handler pointerToWindowsMsg)
{
	const MSG* msg = pointerToWindowsMsg.Get<MSG*>();
	switch (msg->message)
	{

	case WM_TOUCH:
	{
		const UINT inputMsgCount = LOWORD(msg->wParam);
		if (inputMsgCount)
		{
			RECT rect;
			GetClientRect(msg->hwnd, &rect);
			const Math::Vec2<int> screenSize
			{
				rect.right - rect.left,
				rect.bottom - rect.top
			};

			touchInputCache_.resize(inputMsgCount);
			const auto cache = reinterpret_cast<PTOUCHINPUT>(touchInputCache_.data());
			if (GetTouchInputInfo((HTOUCHINPUT)msg->lParam,inputMsgCount, cache,sizeof(TOUCHINPUT)))
			{
				if (!CloseTouchInputHandle((HTOUCHINPUT)msg->lParam))
					throw std::exception{ __FUNCDNAME__ " Cannot close touch input handle." };
			}
			else
			{
				throw std::runtime_error{ std::string{__FUNCDNAME__ " Cannot get touch input info. ID:"} + std::to_string(GetLastError()) };
			}

			const auto convertTouch = [hwnd = msg->hwnd, screenSize](LONG x, LONG y) -> std::optional<Snowing::Math::Vec2f>
			{
				POINT p{ TOUCH_COORD_TO_PIXEL(x),TOUCH_COORD_TO_PIXEL(y) };
				if (!ScreenToClient(hwnd, &p))
					throw std::exception{ __FUNCDNAME__ "Failed to call ScreenToClient." };

				const Snowing::Math::Vec2f posInPixel
				{
					static_cast<float>(p.x),
					static_cast<float>(p.y)
				};

				const Snowing::Math::Vec2f ret = posInPixel / screenSize;

				if (Snowing::Math::IsPositionInBox(ret, Snowing::Math::Vec4f{ 0.0f,0.0f,1.0f,1.0f }))
					return ret;
				else
					return std::nullopt;
			};

			for (UINT i = 0; i < inputMsgCount; ++i)
			{
				const auto& m = cache[i];
				if (m.dwFlags & TOUCHEVENTF_DOWN)
				{
					touchPoints_.push_back({ m.dwID,convertTouch(m.x,m.y)});
				}
				else if (m.dwFlags & TOUCHEVENTF_MOVE)
				{
					for (auto& p : touchPoints_)
					{
						if (p.orginalID == m.dwID)
						{
							p.pos = convertTouch(m.x, m.y);
							break;
						}
					}
				}
				else if (m.dwFlags & TOUCHEVENTF_UP)
				{
					const auto nend = std::remove_if(touchPoints_.begin(), touchPoints_.end(), 
						[id = m.dwID](const TouchPointInfo & p)
					{
						return p.orginalID == id;
					});
					touchPoints_.erase(nend, touchPoints_.end());
				}
			}
		}
		break;
	}
	
	case WM_MOUSEWHEEL:
	{
		const auto delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);
		wheel_ += static_cast<float>(delta) / WHEEL_DELTA;
		break;
	}

	case WM_MOUSEHWHEEL:
	{
		const auto delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);
		hWheel_ += static_cast<float>(delta) / WHEEL_DELTA;
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
		MAPKEY(KeyboardKey::Apps, VK_APPS);
		MAPKEY(KeyboardKey::Up, VK_UP);
		MAPKEY(KeyboardKey::Down, VK_DOWN);
		MAPKEY(KeyboardKey::Left, VK_LEFT);
		MAPKEY(KeyboardKey::Right, VK_RIGHT);
		MAPKEY(KeyboardKey::LShift, VK_LSHIFT);
		MAPKEY(KeyboardKey::RShift, VK_RSHIFT);
		MAPKEY(KeyboardKey::LCtrl, VK_LCONTROL);
		MAPKEY(KeyboardKey::RCtrl, VK_RCONTROL);
		MAPKEY(KeyboardKey::PageDown, VK_PRIOR);
		MAPKEY(KeyboardKey::PageUp, VK_NEXT);
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
	else throw std::invalid_argument(__FUNCDNAME__ "Not Supported Character Keyboard Key");
}
bool Snowing::PlatformImpls::WindowsImpl::InputImpl::KeyPressed(Input::TouchScreen::AnyPoint) const
{
	return !touchPoints_.empty();
}
std::optional<Snowing::Math::Vec2f> Snowing::PlatformImpls::WindowsImpl::InputImpl::Position(Input::MousePosition) const
{
	return mousePosition_;
}

std::optional<Snowing::Math::Vec2f> Snowing::PlatformImpls::WindowsImpl::InputImpl::Position(Input::TouchScreen::AnyPoint) const
{
	if (touchPoints_.empty())
		return std::nullopt;
	else
		return touchPoints_.front().pos;
}

size_t Snowing::PlatformImpls::WindowsImpl::InputImpl::GetMaxTouchInputCount() const
{
	return touchInputCount_;
}
float Snowing::PlatformImpls::WindowsImpl::InputImpl::Trigger(Snowing::Input::MouseWheel) const
{
	return wheel_;
}
float Snowing::PlatformImpls::WindowsImpl::InputImpl::Trigger(Snowing::Input::MouseHWheel) const
{
	return hWheel_;
}
void Snowing::PlatformImpls::WindowsImpl::InputImpl::Update()
{
	wheel_ = 0;
	hWheel_ = 0;
}
#undef MAPKEY

