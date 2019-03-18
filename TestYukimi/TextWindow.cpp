#include "pch.h"
#include <TextWindowFontStyle.h>

using namespace Yukimi;

TEST(TextWindow, FontStyleLoadingCombine)
{
	const TextWindowFontStyle styles[] =
	{
		TextWindowFontStyle::LoadFromProp(LoadAsset("TextWindowFontBase.txt")),
		TextWindowFontStyle::LoadFromProp(LoadAsset("TextWindowFontDeriv.txt"))
	};

	const auto style3 = TextWindowFontStyle::Combine(styles, 2);

	Assert(*style3.Color, styles[1].Color);
	Assert(*style3.Size, styles[1].Size);

	Assert(*style3.ShaderID, styles[0].ShaderID);
	Assert(*style3.AnimationID, styles[0].AnimationID);
}
