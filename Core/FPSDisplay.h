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
		typename TWindow>
		class FPSDisplayInterface final : public Object
	{
	private:
		const Math::Coordinate2DRect screenCoord_ =
		{
			{ 0.0f,0.0f },
			{ float(TWindow::GetSize().x),float(TWindow::GetSize().y) }
		};

		const Math::Vec4f box_ =
		{
			screenCoord_.LeftTop.x,
			screenCoord_.RightBottom.y - 12.0f,
			screenCoord_.RightBottom.x,
			screenCoord_.RightBottom.y
		};

		TBuffer vb_ = TFontRenderer::MakeGPUVertexBuffer();
		TFontRenderer fr_;
		std::vector<Graphics::FontSprite> text_;
		const TFont &font_;

		const std::map<wchar_t, Math::Vec2f> fix_{};

	public:

		FPSDisplayInterface(
			TEffect *effect,
			TTech *tech,
			const TFont *font) :
			fr_{
				&TGraphics::Get().MainContext(),
				effect,
				tech,
				screenCoord_,
				font,
				&vb_
			},
			font_ { *font }
		{}

		bool Update() override
		{
			const float dt = TEngine::Get().DeltaTime();
			const std::wstring s =
				L"FPS:" +
				std::to_wstring(1.0f / dt) +
				L"\nTime:" +
				std::to_wstring(dt * 1000.0f);

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
