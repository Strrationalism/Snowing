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
#include "MenuPositionController.h"

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
	class [[nodiscard]] DebugMenuInterface final: public Object, private NoCopyMove
	{
		class[[nodiscard]] DebugMenuItem final: public UI::TextMenuItemInterface<TFontRenderer>
		{
		public:
			DebugMenuItem(
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
			{}

			void OK()
			{
				func_();
			}
		private:
			const std::function<void()> func_;
		};

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
	Snowing::Input::KeyWatcher<TInput,decltype(KeyValue)> KeyName##_{ &TInput::Get(),KeyValue }
		KEYWATCHER(Snowing::Input::KeyboardKey::Enter, Enter);
		KEYWATCHER(Snowing::Input::KeyboardKey::Up, Up);
		KEYWATCHER(Snowing::Input::KeyboardKey::Down, Down);
		KEYWATCHER(Snowing::Input::MouseKey::Left, MouseLeft);
#undef KEYWATCHER

		Scene::UI::Menu<DebugMenuItem> menu_;
		Scene::UI::MenuKeyController<DebugMenuItem> menuCtrl_{ &menu_ };
		Scene::UI::MenuPositionController<
			DebugMenuItem,
			TInput,
			Snowing::Input::MousePosition> menuMouseCtrl_{
			&menu_,
			screenCoord_,
			&TInput::Get(),
			Snowing::Input::MousePosition::CoordinateSystem,
			Snowing::Input::MousePosition{}
		};

		bool killed_ = false;
		const bool clearScreen_;

	public:
		DebugMenuInterface(
			TTech *tech,
			const TFont *font,
			bool clearScreen = false) :
			fr_{
				&TGraphics::Get().MainContext(),
				tech,
				screenCoord_,
				font,
				&vb_
			},
			font_{ *font },
			clearScreen_{ clearScreen }
		{ 
			menuMouseCtrl_.RefreshSelect();
		}

		~DebugMenuInterface() = default;

		void Kill()
		{
			killed_ = true;
		}

		bool Update() override
		{
			TEngine::Get().Draw([this] {
				if(clearScreen_)
					TGraphics::MainContext().ClearRenderTarget(
						TGraphics::MainRenderTarget());
				TGraphics::MainContext().SetRenderTarget(
					&TGraphics::MainRenderTarget());
				fr_.DrawToSpriteBuffer(text_);
				fr_.FlushSpriteBuffer();
			});

			menu_.Update();
			menuCtrl_.Update();

			Up_.Update();
			if (Up_.JustPress())
			{
				menuCtrl_.Prev();
			}
			Down_.Update();
			if (Down_.JustPress())
			{
				menuCtrl_.Next();
			}
			Enter_.Update();
			MouseLeft_.Update();
			if (Enter_.JustRelease() || MouseLeft_.JustPress())
			{
				if (menu_.GetSelectedIndex().has_value())
					menu_.GetSelectedObject().value()->OK();
			}

			menuMouseCtrl_.Update();

			return !killed_;
		}



		void AddMenuItem(std::wstring_view title, const std::function<void()>& func)
		{
			auto menuBox = Math::Vec4f{
				screenCoord_.LeftTop.x + 20.0f,
				screenCoord_.LeftTop.y + 20.0f * (menu_.Count() + 1),
				2400.0f,
				20.0f
			};

			constexpr Math::Vec2f space{ 1.0f, 75.0f };
			constexpr Math::Vec2f fontSize{ 0.30f,0.30f };
			
			menu_.Emplace(&fr_, title, menuBox, space, fontSize, func);
		}
	};
}