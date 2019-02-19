#pragma once
#include "Platforms.h"
#include "InputParam.h"
#include "Vec2.h"
#include "SingleInstance.h"

namespace Snowing::Graphics
{
	template <typename TImpl>
	class[[nodiscard]] WindowInterface final : public SingleInstance<WindowInterface<TImpl>>
	{
	public:
		[[nodiscard]]
		static bool KeyPressed(Snowing::Input::WindowFocused f)
		{
			return TImpl::Get().KeyPressed(f);
		}

		static void SetWindowed(bool b)
		{
			TImpl::Get().SetWindowed(b);
		}

		static Math::Vec2<int> GetSize()
		{
			return TImpl::Get().GetSize();
		}

		static void Resize(Math::Vec2<int> size)
		{
			TImpl::Get().Resize(size);
		}

		static void ShowCursor(bool cursor)
		{
			TImpl::Get().ShowCursor(cursor);
		}
	};

	
}