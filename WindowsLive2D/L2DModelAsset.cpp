#include "L2DModelAsset.h"
#include <CubismModelSettingJson.hpp>
#include <Model/CubismMoc.hpp>

Live2D::ModelAsset::ModelAsset(const char* homeDir, const char* modelJson, Live2D::AssetLoader loader) :
	loader_{ loader },
	homeDir_{ homeDir },
	modelSetting_{
		std::invoke([this,modelJson] {
			const auto blob = loader_(homeDir_.c_str(), modelJson);

			constexpr size_t CubismModelSettingJsonSizeInner = sizeof(Csm::CubismModelSettingJson);
			static_assert(CubismModelSettingJsonSize >= CubismModelSettingJsonSizeInner);
			
			return new (modelSettingBox_.data()) Csm::CubismModelSettingJson(
				blob.Get<Csm::csmByte*>(),
				static_cast<Csm::csmSizeInt>(blob.Size()));
		}),
		[](void* ptr) {
			static_cast<Csm::CubismModelSettingJson*>(ptr)->
				Csm::CubismModelSettingJson::~CubismModelSettingJson();
		}
	},
	csmMoc_{
		std::invoke([this] {
			const auto moc3Name = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetModelFileName();
			const auto blob = loader_(homeDir_.c_str(),moc3Name);
			return Csm::CubismMoc::Create(
				blob.Get<Csm::csmByte*>(),
				static_cast<Csm::csmSizeInt>(blob.Size()));
		}),
		[](void* ptr) {
			Csm::CubismMoc::Delete(static_cast<Csm::CubismMoc*>(ptr));
		}
	},
	pose_{
		std::invoke([this] () -> std::optional<Snowing::Blob> {
			const auto fileName = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetPoseFileName();
			if (*fileName)
				return loader_(homeDir_.c_str(),fileName);
			else
				return std::nullopt;
		})
	},
	physicsJson_{
		std::invoke([this]() -> std::optional<Snowing::Blob> {
			const auto phyName = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetPhysicsFileName();
			if (*phyName)
				return loader_(homeDir_.c_str(),phyName);
			else
				return std::nullopt;
		})
	},
	expressionJson_{
		std::invoke([this] {
			std::vector<std::pair<std::string_view,Snowing::Blob>> ret;

			Csm::csmInt32 len = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetExpressionCount();
			for (Csm::csmInt32 i = 0; i < len; ++i)
				ret.emplace_back(
					std::make_pair(
						std::string_view{ modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetExpressionName(i)},
						loader_(
							homeDir_.c_str(), 
							modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetExpressionFileName(i))));
			ret.shrink_to_fit();
			return ret;
		})
	},
	expressionNames_{
		std::invoke([this] {
			std::vector<std::string_view> ret;
			ret.reserve(expressionJson_.size());

			for (const auto& i : expressionJson_)
				ret.push_back(i.first);

			ret.shrink_to_fit();
			return ret;
		})
	}
{
	assert(homeDir_.back() == '/');

	// Json
	const auto setting = modelSetting_.Get<Csm::CubismModelSettingJson*>();

	// Load Textures
	{
		const Csm::csmInt32 texCount = setting->GetTextureCount();
		assert(texCount <= MaxTextureCounts);
		for (Csm::csmInt32 i = 0; i < texCount; ++i)
		{
			char path[64];
			strcpy_s(path, setting->GetTextureFileName(i));
			
			// Modify Texture Extract Name
			auto iter = strlen(path);
			assert(iter > 4);
			path[iter - 3] = 'c';
			path[iter - 2] = 't';
			path[iter - 1] = 'x';

			tex_[i].emplace(Snowing::Graphics::LoadTexture(loader_(homeDir_.c_str(),path)));
		}
	}
}

const Live2D::ModelAsset::Handler& Live2D::ModelAsset::GetSetting() const
{
	return modelSetting_;
}

const Live2D::ModelAsset::TextureSet& Live2D::ModelAsset::GetTextures() const
{
	return tex_;
}

const Live2D::ModelAsset::Handler& Live2D::ModelAsset::GetMoc() const
{
	return csmMoc_;
}

const std::optional<Snowing::Blob>& Live2D::ModelAsset::GetPose() const
{
	return pose_;
}

const std::optional<Snowing::Blob>& Live2D::ModelAsset::GetPhysicsJson() const
{
	return physicsJson_;
}

const std::vector<std::pair<std::string_view, Snowing::Blob>>& Live2D::ModelAsset::GetExpressionsJson() const
{
	return expressionJson_;
}

const std::vector<std::string_view>& Live2D::ModelAsset::GetExpressionNames() const
{
	return expressionNames_;
}

const size_t Live2D::ModelAsset::GetExpressionID(std::string_view name) const
{
	for (size_t i = 0; i < expressionNames_.size(); ++i)
		if (expressionNames_[i] == name)
			return i;
	throw std::out_of_range{"Can not find expression in model asset."};
}
