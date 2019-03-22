#pragma once
#include <string>
#include <Snowing.h>
#include "L2DDevice.h"

namespace Live2D
{
	class ModelAsset final : Snowing::NoCopyMove
	{
	public:
		constexpr static size_t MaxTextureCounts = 4;
		using TextureSet = std::array<std::optional<Snowing::Graphics::Texture2D>, MaxTextureCounts>;
		using Handler = Snowing::Platforms::Handler;

	private:
		constexpr static size_t CubismModelSettingJsonSize = 40;

		Live2D::AssetLoader loader_;

		std::string homeDir_;
		std::array<std::uint8_t, CubismModelSettingJsonSize> modelSettingBox_;
		Handler modelSetting_;

		Handler csmMoc_;
		TextureSet tex_;

		std::optional<Snowing::Blob> pose_;

		std::optional<Snowing::Blob> physicsJson_;

	public:
		ModelAsset(const char* homeDir, const char* modelJson, Live2D::AssetLoader = &Snowing::LoadAsset);

		const Handler& GetSetting() const;
		const TextureSet& GetTextures() const;
		const Handler& GetMoc() const;
		const std::optional<Snowing::Blob>& GetPose() const;
		const std::optional<Snowing::Blob>& GetPhysicsJson() const;
	};
}
