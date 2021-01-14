#pragma once
#include "Window.h"
#include "InputImpl.h"
#include "Handler.h"
#include "SingleInstance.h"
namespace Snowing::PlatformImpls::WindowsImpl
{
	struct WindowStyle {
		bool sizable = false;
	};

	class WindowImpl final : public SingleInstance<WindowImpl>
	{
	public:
		using WMCloseHandler = void(*)();
		using WMSizeHandler = Snowing::Math::Vec2<size_t>(*)(Snowing::Math::Vec2<size_t> size);

	private:
		Handler hwnd_;
		InputImpl input_;
		bool keep_ = true;
		bool windowFocused_ = false;
		WMCloseHandler wmCloseHandler_ = nullptr;
		WMSizeHandler wmSizeHandler_ = nullptr;

		Graphics::WindowInterface<PlatformImpls::WindowsImpl::WindowImpl> keepInterface_;

		Math::Vec2<size_t> wndSize_;

		void processWindowMoving();
	public:
		WindowImpl(const WindowImpl&) = delete;
		WindowImpl(WindowImpl&&) = default;
		WindowImpl& operator=(const WindowImpl&) = delete;
		WindowImpl& operator=(WindowImpl&&) = delete;

		void Kill();

		void FocusWindow(bool b);

		WindowImpl(const wchar_t* title,Math::Vec2<size_t> size, WindowStyle windowStyle);

		bool Update();

		InputImpl& GetInputImpl();

		bool KeyPressed(Snowing::Input::WindowFocused) const;

		const Handler& GetHWND() const;

		void SetWindowed(bool windowed);
		void Resize(Math::Vec2<size_t> size);
		void SetTransparent();

		Math::Vec2<size_t> GetSize() const;

		void ShowCursor(bool cursor);

		inline void SetWMCloseHandler(WMCloseHandler handler)
		{
			wmCloseHandler_ = handler;
		}

		inline WMCloseHandler GetWMCloseHandler() const
		{
			return wmCloseHandler_;
		}

		inline void SetWMSizeHandler(WMSizeHandler wm)
		{
			wmSizeHandler_ = wm;
		}

		inline WMSizeHandler GetWMSizeHandler()
		{
			return wmSizeHandler_;
		}
	};

	Math::Vec2<size_t> GetDesktopSize();
	void SetDebuggingScreen(size_t screenId);
}

namespace Snowing::Graphics
{
	using Window = WindowInterface<PlatformImpls::WindowsImpl::WindowImpl>;
}