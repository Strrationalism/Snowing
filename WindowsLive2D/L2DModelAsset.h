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

		const Live2D::AssetLoader loader_;

		const std::string homeDir_;
		std::array<std::uint8_t, CubismModelSettingJsonSize> modelSettingBox_;
		const Handler modelSetting_;

		const Handler csmMoc_;
		TextureSet tex_;

		const std::optional<Snowing::Blob> pose_;

		const std::optional<Snowing::Blob> physicsJson_;

		
		const std::vector<std::pair<std::string_view,Snowing::Blob>> expressionJson_;
		const std::vector<std::string_view> expressionNames_;

		using Motion = Snowing::Blob;
		using MotionGroup = std::pair<std::string_view, std::vector<Motion>>;
		const std::vector<MotionGroup> motionGroup_;

	public:
		ModelAsset(const char* homeDir, const char* modelJson, Live2D::AssetLoader = &DefaultAssetLoader);

		const Handler& GetSetting() const;
		const TextureSet& GetTextures() const;
		const Handler& GetMoc() const;
		const std::optional<Snowing::Blob>& GetPose() const;
		const std::optional<Snowing::Blob>& GetPhysicsJson() const;
		const std::vector<std::pair<std::string_view, Snowing::Blob>>& GetExpressionsJson() const;
		const std::vector<std::string_view>& GetExpressionNames() const;
		const size_t GetExpressionID(std::string_view name) const;
	};
}
