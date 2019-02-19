#pragma once
#include "SMath.h"
#include "Object.h"
#include "FontRenderer.h"

namespace Snowing::Scene
{
	template <
		typename TFontRenderer,
		typename TFont,
		typename TEffect,
		typename TTech,
		typename TGraphics,
		typename TBuffer,
		typename TEngine,
		typename TWindow,
		typename TDataGetter>
		class DebugDisplayInterface final : public Object
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

		const std::map<wchar_t, Math::Vec2f> &fix_;
		const std::wstring title_;
		typename std::decay<TDataGetter>::type getter_;

	public:
		template <typename WString,typename TDataGetter>
		DebugDisplayInterface(
			TEffect *effect,
			TTech *tech,
			const TFont *font,
			const std::map<wchar_t, Math::Vec2f> *fix,
			WString &&title,
			TDataGetter&& dataGetter,
			size_t lineNum) :
			fr_{
				&TGraphics::Get().MainContext(),
				effect,
				tech,
				screenCoord_,
				font,
				&vb_
			},
			font_ { *font },
			fix_ { *fix },
			title_ { std::forward<WString>(title) },
			getter_{ std::forward<TDataGetter>(dataGetter) }
		{
			box_.y -= 12.0f * lineNum;
		}

		DebugDisplayInterface(DebugDisplayInterface&&) = delete;
		DebugDisplayInterface(const DebugDisplayInterface&) = delete;
		DebugDisplayInterface& operator=(DebugDisplayInterface&&) = delete;
		DebugDisplayInterface& operator=(const DebugDisplayInterface&) = delete;

		bool Update() override
		{
			const std::wstring s =
				title_ + L":" +
				To<std::wstring>(getter_());

			Graphics::FontSprite::SetString(
				s,
				font_,
				box_,
				{ 1.0f,75.0f },
				{ 0.15f,0.15f },
				fix_,
				text_,
				Graphics::FontSprite::AlignMode::Right
			);

			TEngine::Get().Draw([this] {
				TGraphics::MainContext().SetRenderTarget(
					&TGraphics::MainRenderTarget());
				fr_.DrawToSpriteBuffer(text_);
				fr_.FlushSpriteBuffer();
			});

			return true;
		}
	};
}
