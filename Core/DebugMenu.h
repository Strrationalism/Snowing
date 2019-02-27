#pragma once
#include <string_view>
#include <functional>
#include "Object.h"
#include "KeyWatcher.h"
#include "KeyKeepTranslator.h"
#include "NoCopyMove.h"
#include "Input.h"
#include "Menu.h"

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
	Snowing::Input::KeyWatcher<TInput,decltype(KeyValue)> KeyName{ &Input::Get(),KeyValue }
		KEYWATCHER(Snowing::Input::KeyboardKey::Enter, Enter);
		KEYWATCHER(Snowing::Input::KeyboardKey::Up, Up);
		KEYWATCHER(Snowing::Input::KeyboardKey::Down, Down);
		KEYWATCHER(Snowing::Input::MouseKey::Left, MouseLeft);
#undef KEYWATCHER

#define KEYKEEPER(keyName) \
	Snowing::Input::KeyKeepTranslator<decltype(keyName)> keyName##Keeper{ &keyName,1.0f,0.2f };

		KEYKEEPER(Up);
		KEYKEEPER(Down);
#undef KEYKEEPER

		class TestMenu : public Scene::Object
		{
		private:
			size_t i_;
		public:
			TestMenu(size_t i) :i_{ i }{}

			void OnSelected()
			{
				Snowing::Log("Menu Selected:", i_);
			}

			void OnUnselected()
			{
				Snowing::Log("Menu Unselected:", i_);
			}
		};

		Scene::UI::Menu<TestMenu> menu_;


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
			// TODO: Key event mouseKey and Enter
			Up.Update();
			if (Up.JustPress())
			{
				menuSelectHelper(Snowing::Input::KeyboardKey::Up);
			}
			Down.Update();
			if (Down.JustPress())
			{
				menuSelectHelper(Snowing::Input::KeyboardKey::Down);
			}
			Enter.Update();
			if (Enter.JustPress())
			{
				
			}




		}

		void menuSelectHelper(Snowing::Input::KeyboardKey key)
		{
			auto menuIndex = menu_.GetSelectedIndex();
			switch(key)
			{
			case Snowing::Input::KeyboardKey::Up:
				
				menu_.Select(menuIndex? menuIndex -1 :menu_.Count() - 1);
				break;
			case Snowing::Input::KeyboardKey::Down:
				menu_.Select(menuIndex == menu_.Count() - 1 ? 0: menuIndex + 1);
				break;
			default:
				Snowing::Log("Menu Operator Unkonwn");
			}
		}


		constexpr Math::Vec2f space{ 1.0f,1.0f };
		constexpr Math::Vec2f fontSize{ 0.5f,0.5f };

		std::map< std::wstring_view, const std::function<void()>&> menuBindEvent;
		
		void AddMenuItem(std::wstring_view title, const std::function<void()>& func)
		{
			auto menuBox = Math::Vec4f{
				screenCoord_.LeftTop.x,
				screenCoord_.LeftTop.y + 50.0f * menu_.Count(),
				800.0f,
				64.0f
			};
			menu_.Emplace(&fr_, title, menuBox, space, fontSize);
			menuBindEvent.emplace(title, func);
		}
	
	private:

	};
}