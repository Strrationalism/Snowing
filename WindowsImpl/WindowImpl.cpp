#include "stdafx.h"
#include "WindowImpl.h"
#include "PlatformImpls.h"
#include <mutex>
#include "COMHelper.h"
#include <stack>

using namespace Snowing::PlatformImpls::WindowsImpl;

static constexpr auto dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#ifdef _DEBUG
#include <iostream>
void Snowing::PlatformImpls::Log(const wchar_t * log)
{
	static std::mutex logLock;
	std::unique_lock l{ logLock };
	std::wcout << log << std::endl;
	OutputDebugStringW(log);
	OutputDebugStringW(L"\n");
}

void Snowing::PlatformImpls::Log(const char * log)
{
	static std::mutex logLock;
	std::unique_lock l{ logLock };
	std::cout << log << std::endl;
	OutputDebugStringA(log);
	OutputDebugStringA("\n");
}

#endif // _DEBUG

constexpr auto MsgBoxType =
MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_APPLMODAL | MB_TOPMOST;

void Snowing::PlatformImpls::Abort(const char * log)
{
	MessageBoxA(
		NULL,
		log,
		"Error",
		MsgBoxType);
	std::quick_exit(-1);
}

void Snowing::PlatformImpls::Abort(const wchar_t * log)
{
	MessageBoxW(
		NULL,
		log,
		L"Error",
		MsgBoxType);
	std::quick_exit(-1);
}

static void hwndDeleter(void* hwnd)
{
	UnregisterTouchWindow((HWND)hwnd);
	wchar_t className[256];
	GetClassName((HWND)hwnd, className, 256);
	DestroyWindow((HWND)hwnd);
	UnregisterClass(className, GetModuleHandle(nullptr));
	CoUninitialize();
}

///////// 这里有一堆用于处理窗口移动的屎
// 当这个变量不存在时，不在移动中
// 当这个变量存在时，记录了鼠标位置到窗口左上角的距离
static std::optional<Snowing::Math::Vec2<int>> movingMousePositionToLTCorner;

static void ProcesSysCommand(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{
	switch (w & 0xFFF0)
	{
	case SC_MOVE:
	{
		RECT r;
		if (!GetWindowRect(wnd, &r))
			throw std::runtime_error{ "Can not get window rect for move." };
		POINT mousePos;
		if (!GetCursorPos(&mousePos))
			throw std::runtime_error{ "Can not get mouse position for move." };

		movingMousePositionToLTCorner.emplace(Snowing::Math::Vec2<int>{
			mousePos.x - r.left,
			mousePos.y - r.top
		});
		break;
	}
	case SC_KEYMENU:
	case SC_MOUSEMENU:
	default:
		DefWindowProc(wnd, msg, w, l);
		break;
	};
}

static WindowStyle windowStyle;
static float sScale = 1.0f;
static std::stack<bool> focusFullScreenStack;
static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{
	auto currentWindow = &WindowImpl::Get();
	switch (msg)
	{
	case WM_MOVE:
	case WM_MOVING:
	case WM_NCRBUTTONDOWN:
		break;

	case WM_SYSCOMMAND:
		ProcesSysCommand(wnd, msg, w, l);
		break;

	case WM_CLOSE:
		if (currentWindow)
		{
			if (currentWindow->GetWMCloseHandler())
				currentWindow->GetWMCloseHandler()();
			else {
				currentWindow->Kill();
				ShowWindow(currentWindow->Get().GetHWND().Get<HWND>(), SW_HIDE);
			}
		}
		break;
	case WM_SETFOCUS:
		if (currentWindow)
		{
			currentWindow->FocusWindow(true);
			currentWindow->GetInputImpl().FocusWindow(true);

			if (!focusFullScreenStack.empty())
			{
				if(focusFullScreenStack.top())
					currentWindow->SetWindowed(false);
				focusFullScreenStack.pop();
			}
		}
		break;
	case WM_KILLFOCUS:
		if (currentWindow) 
		{
			currentWindow->FocusWindow(false);
			currentWindow->GetInputImpl().FocusWindow(false);

			// 这里可以修复一切可能因为Alt+Tab导致的全屏Bug
			if (IsWindowVisible(wnd)) 
			{
				const bool isFullscreen = D3D::Device::Get().GetFullscreen();
				focusFullScreenStack.push(isFullscreen);
				if (isFullscreen)
					currentWindow->SetWindowed(true);
			}
		}
		break;

	case WM_SIZE:
		if (currentWindow) {
			Snowing::Math::Vec2<size_t> size {
				static_cast<size_t>(LOWORD(l)),
				static_cast<size_t>(HIWORD(l))
			};

			if (currentWindow->GetWMSizeHandler())
			{
				size = currentWindow->GetWMSizeHandler()({
					static_cast<size_t>(LOWORD(l)),
					static_cast<size_t>(HIWORD(l))
				});
			}

			//D3D::Device::Get().Resize(size.Cast<int>());

		}
		return DefWindowProc(wnd, msg, w, l);
		break;

	case WM_SIZING:
	{
		RECT orgRect;
		GetWindowRect(wnd, &orgRect);

		constexpr int minWidth = 320;
		const int minHeight = static_cast<int>(320 * sScale);

		int cx, cy;
		LPRECT lpRect = (LPRECT)l;
		if (w == WMSZ_LEFT || w == WMSZ_RIGHT)
		{
			cx = lpRect->right - lpRect->left;
			if (cx < minWidth) {
				cx = minWidth;
				lpRect->right = lpRect->left + cx;
			}
			cy = (int)(cx * sScale);
			lpRect->bottom = lpRect->top + cy;
		}
		else if (w == WMSZ_TOPLEFT || w == WMSZ_BOTTOMLEFT)
		{
			cy = lpRect->bottom - lpRect->top;
			cx = (int)(cy / sScale);

			if (cy < minHeight) {
				cy = minHeight;
				lpRect->bottom = lpRect->top + cy;
			}

			if (cx < minWidth) {
				cx = minWidth;
				lpRect->right = lpRect->left + cx;
			}

			lpRect->left = lpRect->right - cx;
		}
		else
		{
			cy = lpRect->bottom - lpRect->top;
			if (cy < minHeight) {
				cy = minHeight;
				lpRect->bottom = lpRect->top + cy;
			}
			cx = (int)(cy / sScale);
			lpRect->right = lpRect->left + cx;
		}

		return DefWindowProc(wnd, msg, w, l);
	}
	
	default:{
		MSG msgs
		{
			wnd,
			msg,
			w,
			l
		};
		if (currentWindow)
			if (!currentWindow->GetInputImpl().PushMessage(Handler{ &msgs,Handler::DoNothingDeleter }))
				return DefWindowProc(wnd, msg, w, l);
	}
	};
	return 0;
}



void Snowing::PlatformImpls::WindowsImpl::WindowImpl::Kill()
{
	keep_ = false;
}

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::FocusWindow(bool b)
{
	windowFocused_ = b;
}

#ifdef _DEBUG
static size_t debuggingScreenId = 0;
#endif

Snowing::PlatformImpls::WindowsImpl::WindowImpl::WindowImpl(const wchar_t* title, Math::Vec2<size_t> size, WindowStyle windowStyle):
	wndSize_{ size }
{
	::windowStyle = windowStyle;
	COMHelper::AssertHResult("CoInitializeEx failed!",
		CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED));

	Library{ "Imm32.dll" }
		.GetCast<BOOL(__stdcall*)(IN DWORD)>(
			"ImmDisableIME")(0);

	auto instance = GetModuleHandle(NULL);//得到程序实例句柄

	const auto winpos = GetDesktopSize() / 2 - size / 2;
	RECT winRect =
	{
		static_cast<LONG>(winpos.x), static_cast<LONG>(winpos.y),
		static_cast<LONG>(winpos.x + size.x), static_cast<LONG>(winpos.y + size.y)
	};

#ifdef _DEBUG
	std::vector<RECT> monitors;
	EnumDisplayMonitors(nullptr, nullptr, 
		[] (HMONITOR, HDC, LPRECT lprcMonitor, LPARAM dwData) -> BOOL{
			static_assert(sizeof(dwData) == sizeof(void*));
			const auto monitors = reinterpret_cast<std::vector<RECT>*>(dwData);
			monitors->push_back(*lprcMonitor);
			return TRUE;
		}, reinterpret_cast<LPARAM>(&monitors));
	const size_t debuggingMonitorIdSelected = std::clamp<size_t>(debuggingScreenId, 0u, monitors.size() - 1);
	const auto screenRect = monitors.at(debuggingMonitorIdSelected);
	const Snowing::Math::Vec2<LONG> screenSize = {
		screenRect.right - screenRect.left,
		screenRect.bottom - screenRect.top
	};
	const auto lSize = size.Cast<LONG>();
	winRect = {
		screenRect.left + screenSize.x / 2 - lSize.x / 2, screenRect.top + screenSize.y / 2 - lSize.y / 2,
		screenRect.left + screenSize.x / 2 + lSize.x / 2, screenRect.top + screenSize.y / 2 + lSize.y / 2
	};
#endif // _DEBUG


	if (!AdjustWindowRect(&winRect, dwStyle, false))
		throw std::runtime_error{ "AdjustWindowRect error" };

	sScale = (winRect.bottom - winRect.top) / static_cast<float>(winRect.right - winRect.left);

	WNDCLASSEX wnd;
	wnd.cbClsExtra = 0;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd.hIconSm = wnd.hIcon;
	wnd.hInstance = instance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = title;
	wnd.lpszMenuName = title;
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	
	if (windowStyle.icon.has_value())
	{
		const auto icon = static_cast<HICON>(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(*windowStyle.icon)));
		icon_ = {
			icon,
			[](void* icon) {
				DestroyIcon(static_cast<HICON>(icon));
			}
		};

		wnd.hIcon = icon;
		wnd.hIconSm = icon;
	}

	if (!RegisterClassEx(&wnd))
	{
		throw std::runtime_error("Failed to register the window.");
	}

	const auto hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		title,
		title,
		dwStyle,
		winRect.left, winRect.top,
		winRect.right - winRect.left, winRect.bottom - winRect.top,
		NULL, NULL,
		instance, NULL);

	// 注册为触摸窗口
	RegisterTouchWindow(hwnd, 0);

	// 注册为Sizable窗口
	if (windowStyle.sizable) {
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_SIZEBOX);
	}

	hwnd_ = Handler{ hwnd, hwndDeleter };
}

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::processWindowMoving()
{
	if (movingMousePositionToLTCorner.has_value())
	{
		// 如果鼠标松开则退出窗口移动模式
		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8001) == 0)
			movingMousePositionToLTCorner.reset();
		else
		{

			POINT mousePoint;
			if (!GetCursorPos(&mousePoint))
				throw std::runtime_error{ "Can not get mouse point for move." };
			RECT r;
			if (!GetWindowRect(hwnd_.Get<HWND>(), &r))
				throw std::runtime_error{ "Can not get window rect for move." };
			MoveWindow(
				hwnd_.Get<HWND>(),
				mousePoint.x - movingMousePositionToLTCorner->x,
				mousePoint.y - movingMousePositionToLTCorner->y,
				r.right - r.left,
				r.bottom - r.top,
				FALSE);
		}
	}
}

bool Snowing::PlatformImpls::WindowsImpl::WindowImpl::Update()
{
	processWindowMoving();

	MSG msg = { 0 };

	while(PeekMessage(&msg, hwnd_.Get<HWND>(), 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
	}

	return keep_;
}

Snowing::PlatformImpls::WindowsImpl::InputImpl & Snowing::PlatformImpls::WindowsImpl::WindowImpl::GetInputImpl() 
{
	return input_;
}

bool Snowing::PlatformImpls::WindowsImpl::WindowImpl::KeyPressed(Input::WindowFocused) const
{
	return windowFocused_;
}

const Handler & Snowing::PlatformImpls::WindowsImpl::WindowImpl::GetHWND() const
{
	return hwnd_;
}

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::SetWindowed(bool windowed)
{
	D3D::Device::Get().SetFullscreen(!windowed);

	if (windowed)
	{
		HWND h = hwnd_.Get<HWND>();
		ShowWindow(h, SW_SHOW);
	}
}

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::Resize(Math::Vec2<size_t> size)
{
	wndSize_ = size;
	const auto winpos = GetDesktopSize() / 2 - size / 2;
	RECT winRect =
	{
		static_cast<LONG>(winpos.x), static_cast<LONG>(winpos.y),
		static_cast<LONG>(winpos.x + size.x), static_cast<LONG>(winpos.y + size.y)
	};


	if (D3D::Device::InstanceExists()) {
		if (!D3D::Device::Get().GetFullscreen())
			if (!AdjustWindowRect(&winRect, dwStyle, false))
				throw std::runtime_error{ "AdjustWindowRect error" };
	}
	else {
		if (!AdjustWindowRect(&winRect, dwStyle, false))
			throw std::runtime_error{ "AdjustWindowRect error" };
	}

	MoveWindow(hwnd_.Get<HWND>(),winRect.left,winRect.top,winRect.right - winRect.left,winRect.bottom - winRect.top,true);

	D3D::Device::Get().Resize(size.Cast<int>());
}


#include <dwmapi.h>
#include "LibraryImpl.h"
#include "COMHelper.h"
void Snowing::PlatformImpls::WindowsImpl::WindowImpl::SetTransparent()
{
	SetWindowLong(
		hwnd_.Get<HWND>(),
		GWL_STYLE,
		GetWindowLong(hwnd_.Get<HWND>(),GWL_STYLE) 
			&~WS_CAPTION 
			&~WS_SYSMENU 
			&~WS_SIZEBOX
			&~WS_MINIMIZEBOX 
			&~WS_SYSMENU
			&~WS_BORDER
			&~WS_POPUP);

	SetWindowLong(
		hwnd_.Get<HWND>(),
		GWL_EXSTYLE,
		(GetWindowLong(hwnd_.Get<HWND>(), GWL_EXSTYLE)
			&~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED);

	Library lib("dwmapi.dll");

	MARGINS m = { -1,-1,-1,-1 };
	typedef HRESULT(__stdcall *Func)(HWND, const MARGINS *);
	COMHelper::AssertHResult("Can not set transparent.",
		lib.GetCast<Func>("DwmExtendFrameIntoClientArea")(
			hwnd_.Get<HWND>(), &m));

	RECT rcClient;
	GetWindowRect(hwnd_.Get<HWND>(), &rcClient);

	SetWindowPos(
		hwnd_.Get<HWND>(),
		HWND_TOPMOST,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		SWP_FRAMECHANGED | SWP_NOMOVE);
}

Snowing::Math::Vec2<size_t> Snowing::PlatformImpls::WindowsImpl::WindowImpl::GetSize() const
{
	/*RECT r; //这里需要进一步讨论，在某些情况下GetClientRect返回四个0
	if (!GetClientRect(hwnd_.Get<HWND>(), &r))
		throw std::exception{ __FUNCDNAME__ "Faild to call GetClientRect." };
	return { r.right - r.left,r.bottom - r.top };*/
	return wndSize_;
}

Snowing::Math::Vec2<size_t> Snowing::PlatformImpls::WindowsImpl::GetDesktopSize()
{
	// 旧版获得窗口大小，受到DPI影响
	/*RECT desktopRect;
	const auto desktop = GetDesktopWindow();
	if (!GetClientRect(desktop, &desktopRect))
		throw std::runtime_error("Get desktop size failed.");
	return {
		desktopRect.right - desktopRect.left,
		desktopRect.bottom - desktopRect.top };*/

	return {
		static_cast<size_t>(GetSystemMetrics(SM_CXSCREEN)),
		static_cast<size_t>(GetSystemMetrics(SM_CYSCREEN))
	};

}

void Snowing::PlatformImpls::WindowsImpl::SetDebuggingScreen(size_t screenId)
{
#ifdef _DEBUG
	debuggingScreenId = screenId;
#endif
}

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::ShowCursor(bool cursor)
{
	::ShowCursor(cursor);
}

