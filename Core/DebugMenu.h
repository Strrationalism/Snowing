#pragma once
#include <string_view>
#include <functional>
#include "Object.h"
#include "KeyWatcher.h"
#include "KeyKeepTranslator.h"
#include "NoCopyMove.h"
#include "Input.h"
#include "Menu.h"
#include "TextMenuItem.h"
#include "MenuKeyController.h"

namespace Snowing::Scene::Debug
{
	template<typename TFontRenderer,
		typename TFont,
		typename TEffect,
		typename TTech,
		typename TGraphics,
		typename TBuffer,
		typename TEngine,
		typename TWindow,
		typename TInput>
	class DebugMenuInterface: public Object, private NoCopyMove
	{
		template <typename TFontRenderer>
		class[[nodiscard]] TestMenuItemInterface : public UI::TextMenuItemInterface<TFontRenderer>
		{
		public:

			TestMenuItemInterface(
				TFontRenderer *fontRenderer,
				std::wstring_view text,
				Math::Vec4f box,
				Math::Vec2f space,
				Math::Vec2f fontSize,
				const std::function<void()>& func
			) : UI::TextMenuItemInterface<TFontRenderer>
			{
				fontRenderer,
				text,
				box,
				space,
				fontSize},
				func_{func}
			{

			}

			void OK()
			{
				func_();
			}
		private:
			const std::function<void()> func_;
		};

		using TestMenuItem = TestMenuItemInterface< TFontRenderer>;

	private:
		const Math::Coordinate2DRect screenCoord_ =
		{
			{ 0.0f,0.0f },
			{ float(TWindow::GetSize().x),float(TWindow::GetSize().y) }
		};

		Math::Vec4f box_ =
		{
			screenCoord_.LeftTop.x,
			screenCoord_.RightBottom.y,
			screenCoord_.RightBottom.x,
			screenCoord_.RightBottom.y
		};

		TBuffer vb_ = TFontRenderer::MakeGPUVertexBuffer();
		TFontRenderer fr_;
		std::vector<Graphics::FontSprite> text_;
		const TFont &font_;

#define KEYWATCHER(KeyValue,KeyName) \
	Snowing::Input::KeyWatcher<TInput,decltype(KeyValue)> KeyName{ &TInput::Get(),KeyValue }
		KEYWATCHER(Snowing::Input::KeyboardKey::Enter, Enter);
		KEYWATCHER(Snowing::Input::KeyboardKey::Up, Up);
		KEYWATCHER(Snowing::Input::KeyboardKey::Down, Down);
#undef KEYWATCHER

		Scene::UI::Menu<TestMenuItem> menu_;
		Scene::UI::MenuKeyController<TestMenuItem> menuKeyController{ &menu_ };

	public:
		DebugMenuInterface(
			TTech *tech,
			const TFont *font) :
			fr_{
				&TGraphics::Get().MainContext(),
				tech,
				screenCoord_,
				font,
				&vb_
			},
			font_{ *font }
		{

		}
		~DebugMenuInterface()
		{
			
		}

		

		bool Update() override
		{
			Object::Update();

			TEngine::Get().Draw([this] {
				TGraphics::MainContext().ClearRenderTarget(
					TGraphics::MainRenderTarget());
				TGraphics::MainContext().SetRenderTarget(
					&TGraphics::MainRenderTarget());
				fr_.DrawToSpriteBuffer(text_);
				fr_.FlushSpriteBuffer();
			});


			menu_.Update();
			menuKeyController.Update();

			Up.Update();
			if (Up.JustPress())
			{
				menuKeyController.Prev();
			}
			Down.Update();
			if (Down.JustPress())
			{
				menuKeyController.Next();
			}
			Enter.Update();
			if (Enter.JustPress())
			{
				menu_.GetSelectedObject().value()->OK();
			}

			return true;
		}

		Math::Vec2f space{ 1.0f,1.0f };
		Math::Vec2f fontSize{ 0.5f,0.5f };

		std::map< std::wstring_view, const std::function<void()>&> menuBindEvent;

		void AddMenuItem(std::wstring_view title, const std::function<void()>& func)
		{
			auto menuBox = Math::Vec4f{
				screenCoord_.LeftTop.x + 50.0f,
				screenCoord_.LeftTop.y + 50.0f * (menu_.Count() + 1),
				800.0f,
				64.0f
			};
			

			menu_.Emplace(&fr_, title, menuBox, space, fontSize, func);
		}
	
	private:

	};
}