#include "pch.h"
#include "TextWindowFontStyle.h"

using namespace Yukimi;

TextWindowFontStyle TextWindowFontStyle::LoadFromProp(const Snowing::PropParser<char>& p)
{
	const auto r = p.GetOpt<float>("r");
	const auto g = p.GetOpt<float>("g");
	const auto b = p.GetOpt<float>("b");
	std::optional<Snowing::Math::Vec3f> color;

	if (r.has_value() && g.has_value() && b.has_value())
		color = Snowing::Math::Vec3f{ *r,*g,*b };

	return
	{
		p.GetOpt<float>("size"),
		Snowing::To<Snowing::BKDRHash>(p.GetOpt("animation")),
		Snowing::To<Snowing::BKDRHash>(p.GetOpt("shader")),
		color,
		p.GetOpt<float>("stroke")
	};
}

TextWindowFontStyle TextWindowFontStyle::Combine(TextWindowFontStyle ret, const TextWindowFontStyle& deriv)
{
	if (deriv.Size.has_value())
		ret.Size = deriv.Size;

	if (deriv.AnimationID.has_value())
		ret.AnimationID.emplace(*deriv.AnimationID);

	if (deriv.ShaderID.has_value())
		ret.ShaderID.emplace(*deriv.ShaderID);

	if (deriv.Color.has_value())
		ret.Color = deriv.Color;

	if (deriv.Stroke.has_value())
		ret.Stroke = deriv.Stroke;

	return ret;
}

TextWindowFontStyle TextWindowFontStyle::LoadFromProp(const Snowing::Blob& p)
{
	return LoadFromProp(Snowing::PropParser<char>{ &p });
}

TextWindowFontStyle TextWindowFontStyle::Combine(const TextWindowFontStyle* styles, size_t count)
{
	assert(styles);
	assert(count);

	auto ret = *styles;
	for (size_t i = 0; i < count - 1; ++i)
		ret = Combine(ret, styles[i + 1]);
	return ret;
}

TextWindowFontStyle& TextWindowFontStyle::operator = (const TextWindowFontStyle& a)
{
	Size = a.Size;
	AnimationID.emplace(*a.AnimationID);
	ShaderID.emplace(*a.ShaderID);
	Color = a.Color;
	Stroke = a.Stroke;

	return *this;
}
