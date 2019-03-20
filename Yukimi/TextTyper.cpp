#include "pch.h"
#include <algorithm>
#include "TextTyper.h"

using namespace Yukimi;
using namespace Snowing;

TextTyper::TextTyper(
	const Snowing::Graphics::Font* fnt,
	Snowing::Math::Vec4f box,
	Snowing::Math::Vec2f fontSize,
	Snowing::Math::Vec2f space,
	const std::map<wchar_t, Snowing::Math::Vec2f>* fix):
	font_{ fnt },
	box_{ box },
	space_{ space },
	positionFix_{ fix },
	fontSize_{ fontSize }
{
	Reset();
}

void TextTyper::Reset()
{
	xPos_ = box_.x - space_.y * fontSize_.y / 2;
	yPos_ = box_.y + space_.y * fontSize_.y;
}

void TextTyper::NextLine()
{
	xPos_ = box_.x - space_.y * fontSize_.y / 2;
	yPos_ += space_.y * fontSize_.y;
}

Snowing::Math::Vec2f TextTyper::Type(wchar_t c,float size)
{
	if (c == ' ')
	{
		xPos_ += 16 * space_.x * space_.x;
		return { 0,0 };
	}
	else if (c == '\n')
	{
		NextLine();
		return { 0,0 };
	}
	else
	{
		Math::Vec2f fix = { 0.0f,0.0f };
		if (positionFix_->count(c))
			fix = positionFix_->at(c);


		auto [faceID, spID] = font_->Charaters.at(c);

		auto xMove =
			(fix.x + space_.x) *
			font_->SpriteSheetsCPU[faceID].Sheet[spID].z *
			font_->Faces[faceID].Size().x *
			fontSize_.x * size;

		if (xPos_ + xMove > box_.z)
			NextLine();

		Math::Vec2f pos { xPos_ ,yPos_ };
		xPos_ += xMove;
		return pos;
	}
}