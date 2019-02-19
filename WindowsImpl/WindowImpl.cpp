#include "stdafx.h"
#include "WindowImpl.h"
#include "PlatformImpls.h"

using namespace Snowing::PlatformImpls::WindowsImpl;

static constexpr auto dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

#ifdef _DEBUG
#include <iostream>
void Snowing::PlatformImpls::Log(const wchar_t * log)
{
	std::wcout << log << std::endl;
	OutputDebugStringW(log);
	OutputDebugStringW(L"\n");
}

void Snowing::PlatformImpls::Log(const char * log)
{
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
		WindowsImpl::WindowImpl::Get().GetHWND().Get<HWND>(),
		log,
		"Error",
		MsgBoxType);
	std::terminate();
}

void Snowing::PlatformImpls::Abort(const wchar_t * log)
{
	MessageBoxW(
		WindowsImpl::WindowImpl::Get().GetHWND().Get<HWND>(),
		log,
		L"Error",
		MsgBoxType);
	std::terminate();
}

static void hwndDeleter(void* hwnd)
{
	wchar_t className[256];
	GetClassName((HWND)hwnd, className, 256);
	DestroyWindow((HWND)hwnd);
	UnregisterClass(className, GetModuleHandle(nullptr));
}

static void ProcesSysCommand(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{
	switch (w & 0xFFF0)
	{
	case SC_KEYMENU:
	case SC_MOUSEMENU:
		break;
	default:
		DefWindowProc(wnd, msg, w, l);
		break;
	};
}

static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{
	auto currentWindow = &WindowImpl::Get();
	switch (msg)
	{
	case WM_NCRBUTTONDOWN:
		break;

	case WM_SYSCOMMAND:
		ProcesSysCommand(wnd, msg, w, l);
		break;


	case WM_CLOSE:
		if (currentWindow)
			if (currentWindow->GetWMCloseHandler())
				currentWindow->GetWMCloseHandler()();
			else
				currentWindow->Kill();
			
		break;
	case WM_SETFOCUS:
		if (currentWindow)
		{
			currentWindow->FocusWindow(true);
			currentWindow->GetInputImpl().FocusWindow(true);
		}
		break;
	case WM_KILLFOCUS:
		if (currentWindow) 
		{
			currentWindow->FocusWindow(false);
			currentWindow->GetInputImpl().FocusWindow(false);
		}
		break;
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

Snowing::PlatformImpls::WindowsImpl::WindowImpl::WindowImpl(const wchar_t* title, Math::Vec2<int> size)
{
	auto instance = GetModuleHandle(NULL);//得到程序实例句柄

	const auto winpos = GetDesktopSize() / 2 - size / 2;
	RECT winRect =
	{
		winpos.x,winpos.y,
		winpos.x + size.x,winpos.y + size.y
	};

	if (!AdjustWindowRect(&winRect, dwStyle, false))
		throw std::runtime_error{ "AdjustWindowRect error" };

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

	// TestHandler
	Handler hnd(hwnd, hwndDeleter);
	Handler hnd2 = std::move(hnd);
	hwnd_ = std::move(hnd2);
}

bool Snowing::PlatformImpls::WindowsImpl::WindowImpl::Update()
{
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

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::Resize(Math::Vec2<int> size)
{
	const auto winpos = GetDesktopSize() / 2 - size / 2;
	RECT winRect =
	{
		winpos.x,winpos.y,
		winpos.x + size.x,winpos.y + size.y
	};

	if (!AdjustWindowRect(&winRect, dwStyle, false))
		throw std::runtime_error{ "AdjustWindowRect error" };

	MoveWindow(hwnd_.Get<HWND>(),winRect.left,winRect.top,winRect.right - winRect.left,winRect.bottom - winRect.top,true);

	D3D::Device::Get().Resize(size);
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

	SetWindowPos(
		hwnd_.Get<HWND>(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

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
		nullptr,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		SWP_FRAMECHANGED);
}

Snowing::Math::Vec2<int> Snowing::PlatformImpls::WindowsImpl::GetDesktopSize()
{
	RECT desktopRect;
	const auto desktop = GetDesktopWindow();
	if (!GetClientRect(desktop, &desktopRect))
		throw std::runtime_error("Get desktop size failed.");
	return {
		desktopRect.right - desktopRect.left,
		desktopRect.bottom - desktopRect.top };
}

void Snowing::PlatformImpls::WindowsImpl::WindowImpl::ShowCursor(bool cursor)
{
	::ShowCursor(cursor);
}

