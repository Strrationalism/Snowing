#pragma once
#include <string>
#include "L2DDevice.h"

namespace Live2D
{
	class ModelAsset final : Snowing::NoCopyMove
	{
	private:
#
		constexpr static size_t CubismModelSettingJsonSize = 40;
		constexpr static size_t MaxTextureCounts = 4;

		Live2D::AssetLoader loader_;

		std::string homeDir_;
		std::array<std::uint8_t, CubismModelSettingJsonSize> modelSettingBox_;
		Snowing::Platforms::Handler modelSetting_;

		Snowing::Platforms::Handler csmMoc_;
		std::array<std::optional<Snowing::Graphics::Texture2D>, MaxTextureCounts> tex_;


	public:
		ModelAsset(const char* homeDir, const char* modelJson, Live2D::AssetLoader = &Snowing::LoadAsset);
	};
}
