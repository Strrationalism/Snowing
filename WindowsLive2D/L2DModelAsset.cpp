#include "L2DModelAsset.h"
#include <CubismModelSettingJson.hpp>
#include <Model/CubismMoc.hpp>

Live2D::ModelAsset::ModelAsset(const char* homeDir, const char* modelJson, Live2D::AssetLoader loader) :
	homeDir_{ homeDir },
	modelSetting_{
		std::invoke([this,modelJson,loader] {
			const auto blob = loader(homeDir_.c_str(), modelJson);

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
		std::invoke([this,loader] {
			const auto moc3Name = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetModelFileName();
			const auto blob = loader(homeDir_.c_str(),moc3Name);
			return Csm::CubismMoc::Create(
				blob.Get<Csm::csmByte*>(),
				static_cast<Csm::csmSizeInt>(blob.Size()));
		}),
		[](void* ptr) {
			Csm::CubismMoc::Delete(static_cast<Csm::CubismMoc*>(ptr));
		}
	},
	pose_{
		std::invoke([this,loader] () -> std::optional<Snowing::Blob> {
			const auto fileName = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetPoseFileName();
			if (*fileName)
				return loader(homeDir_.c_str(),fileName);
			else
				return std::nullopt;
		})
	},
	physicsJson_{
		std::invoke([this,loader]() -> std::optional<Snowing::Blob> {
			const auto phyName = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetPhysicsFileName();
			if (*phyName)
				return loader(homeDir_.c_str(),phyName);
			else
				return std::nullopt;
		})
	},
	expressionJson_{
		std::invoke([this,loader] {
			std::vector<std::pair<std::string_view,Snowing::Blob>> ret;

			Csm::csmInt32 len = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetExpressionCount();
			for (Csm::csmInt32 i = 0; i < len; ++i)
				ret.emplace_back(
					std::make_pair(
						std::string_view{ modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetExpressionName(i)},
						loader(
							homeDir_.c_str(), 
							modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetExpressionFileName(i))));
			ret.shrink_to_fit();
			return ret;
		})
	},
	motionGroup_{
		std::invoke([this,loader] {
			const auto setting = modelSetting_.Get<Csm::CubismModelSettingJson*>();
			const Csm::csmInt32 groupCount = setting->GetMotionGroupCount();
			std::vector<MotionGroup> motionGroup_;

			for (Csm::csmInt32 groupID = 0; groupID < groupCount; ++groupID)
			{
				const auto groupName = setting->GetMotionGroupName(groupID);
				motionGroup_.emplace_back(std::make_pair(groupName,std::vector<Motion>{}));
				auto& curGroup = motionGroup_.back().second;
				const auto motionCount = setting->GetMotionCount(groupName);

				for (Csm::csmInt32 motionID = 0; motionID < motionCount; ++motionID)
				{
					curGroup.emplace_back(
						loader(homeDir_.c_str(),setting->GetMotionFileName(groupName, motionID)));
				}
				curGroup.shrink_to_fit();
			}
			motionGroup_.shrink_to_fit();
			return motionGroup_;
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

			tex_[i].emplace(Snowing::Graphics::LoadTexture(loader(homeDir_.c_str(),path)));
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

const std::vector<std::pair<std::string_view, Snowing::Blob>>& Live2D::ModelAsset::GetExpressions() const
{
	return expressionJson_;
}

const size_t Live2D::ModelAsset::GetExpressionID(std::string_view name) const
{
	for (size_t i = 0; i < expressionJson_.size(); ++i)
		if (expressionJson_[i].first == name)
			return i;
	throw std::out_of_range{"Can not find expression in model asset."};
}
