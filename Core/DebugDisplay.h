#pragma once
#include <functional>
#include "SMath.h"
#include "Object.h"
#include "FontRenderer.h"
#include "NoCopyMove.h"

namespace Snowing::Scene::Debug
{
	template <
		typename TFontRenderer,
		typename TFont,
		typename TEffect,
		typename TTech,
		typename TGraphics,
		typename TBuffer,
		typename TEngine,
		typename TWindow>
	class DebugDisplayInterface final : public Object, private NoCopyMove
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

		const std::wstring title_;
		std::function<std::wstring()> getter_;

		constexpr static size_t MaxLines = 100;

		static inline bool lineNums_[MaxLines];
		size_t lineNum_;

	public:
		template <typename WString>
		DebugDisplayInterface(
			TTech *tech,
			const TFont *font,
			WString &&title,
			const std::function<std::wstring()>& dataGetter) :
			fr_{
				&TGraphics::Get().MainContext(),
				tech,
				screenCoord_,
				font,
				&vb_
			},
			font_ { *font },
			title_ { std::forward<WString>(title) },
			getter_{ dataGetter }
		{
			for (lineNum_ = 0; lineNum_ < 100; ++lineNum_)
			{
				if (!lineNums_[lineNum_])
				{
					lineNums_[lineNum_] = true;
					break;
				}
			}
			box_.y -= 12.0f * lineNum_;
		}

		~DebugDisplayInterface()
		{
			lineNums_[lineNum_] = false;
		}

		bool Update() override
		{
			const std::wstring s =
				title_ + L":" +
				getter_();

			Graphics::FontSprite::SetString(
				s,
				font_,
				box_,
				{ 1.0f,75.0f },
				{ 0.2f,0.2f },
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

		static const inline std::function<std::wstring()> FrameTimeGetter = [] {
			return To<std::wstring>(TEngine::Get().DeltaTime() * 1000);
		};

		static const inline std::function<std::wstring()> FPSGetter = [] {
			return To<std::wstring>(1 / TEngine::Get().DeltaTime());
		};
	};
}
