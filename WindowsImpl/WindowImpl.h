#pragma once
#include "Window.h"
#include "InputImpl.h"
#include "Handler.h"
#include "SingleInstance.h"
namespace Snowing::PlatformImpls::WindowsImpl
{
	class WindowImpl final : public SingleInstance<WindowImpl>
	{
	public:
		using WMCloseHandler = void(*)();

	private:
		Handler hwnd_;
		InputImpl input_;
		bool keep_ = true;
		bool windowFocused_ = false;
		WMCloseHandler wmCloseHandler_ = nullptr;

		Graphics::WindowInterface<PlatformImpls::WindowsImpl::WindowImpl> keepInterface_;
	public:
		WindowImpl(const WindowImpl&) = delete;
		WindowImpl(WindowImpl&&) = default;
		WindowImpl& operator=(const WindowImpl&) = delete;
		WindowImpl& operator=(WindowImpl&&) = delete;

		void Kill();

		void FocusWindow(bool b);

		WindowImpl(const wchar_t* title,Math::Vec2<int> size);

		bool Update();

		InputImpl& GetInputImpl();

		bool KeyPressed(Snowing::Input::WindowFocused) const;

		const Handler& GetHWND() const;

		void SetWindowed(bool windowed);
		void Resize(Math::Vec2<int> size);

		Math::Vec2<int> GetSize() const;

		void ShowCursor(bool cursor);

		inline void SetWMCloseHandler(WMCloseHandler handler)
		{
			wmCloseHandler_ = handler;
		}

		inline WMCloseHandler GetWMCloseHandler() const
		{
			return wmCloseHandler_;
		}
	};
}

namespace Snowing::Graphics
{
	using Window = WindowInterface<PlatformImpls::WindowsImpl::WindowImpl>;
}