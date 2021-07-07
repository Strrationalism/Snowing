#pragma once
#include <PlatformImpls.h>

namespace Yukimi
{
	class TextTyper final
	{
	private:
		const Snowing::Graphics::Font * const font_;
		Snowing::Math::Vec4f box_;
		const Snowing::Math::Vec2f space_;
		const Snowing::Math::Vec2f fontSize_;
		const std::map<wchar_t, Snowing::Math::Vec2f>* const positionFix_;

		float xPos_;
		float yPos_;

	public:
		TextTyper(
			const Snowing::Graphics::Font*,
			Snowing::Math::Vec4f box,
			Snowing::Math::Vec2f fontSize,
			Snowing::Math::Vec2f space);

		void SetBox(Snowing::Math::Vec4f box) { box_ = box; }

		void NextLine();
		Snowing::Math::Vec2f Type(wchar_t ch,float size = 1);
		void Reset();
	};
}
