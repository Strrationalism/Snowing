#pragma once
#include "Platforms.h"
#include "Object.h"
#include "FontRenderer.h"

namespace Snowing::Scene::UI
{
	template <typename TFontRenderer>
	class[[nodiscard]] TextMenuItemInterface : public Object
	{
	private:

		TFontRenderer *fontRenderer_;
		std::vector<Graphics::FontSprite> str_;
		const Math::Vec4f box_;

		constexpr static size_t MagicFontSize = 128;

	public:

		void OnUnselected()
		{
			for (auto& p : str_)
				p.Sprite.Color = { 0.5f,0.5f,0.5f,1.0f };
		}

		void OnSelected()
		{
			for (auto& p : str_)
				p.Sprite.Color = { 1.0f,1.0f,1.0f,1.0f };
		}

		Math::Vec4f GetBox() const
		{
			return box_;
		}

		TextMenuItemInterface(
			TFontRenderer *fontRenderer, 
			std::wstring_view text,
			Math::Vec4f box,
			Math::Vec2f space,
			Math::Vec2f fontSize,
			Graphics::FontSprite::AlignMode alignMode = Graphics::FontSprite::AlignMode::Left):
			fontRenderer_{ fontRenderer },
			box_{ box - Math::Vec4f{0.0f,fontSize.y * space.y * MagicFontSize,0.0f,0.0f } }
		{
			Graphics::FontSprite::SetString(
				text,
				fontRenderer->GetFont(),
				box,
				space,
				fontSize,
				str_,
				alignMode);
			OnUnselected();
		}

		bool Update() override
		{
			fontRenderer_->DrawToSpriteBuffer(str_);
			fontRenderer_->FlushSpriteBuffer();
			return true;
		}
	};
}
