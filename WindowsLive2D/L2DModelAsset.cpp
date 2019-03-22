#include "L2DModelAsset.h"
#include <CubismModelSettingJson.hpp>
#include <Model/CubismMoc.hpp>
#include <Effect/CubismPose.hpp>

Live2D::ModelAsset::ModelAsset(const char* homeDir, const char* modelJson, Live2D::AssetLoader loader) :
	loader_{ loader },
	homeDir_{ homeDir },
	modelSetting_{
		std::invoke([this,modelJson] {
			const auto blob = loader_((homeDir_ + modelJson).c_str());

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
			const auto blob = loader_((homeDir_ + moc3Name).c_str());
			return Csm::CubismMoc::Create(
				blob.Get<Csm::csmByte*>(),
				static_cast<Csm::csmSizeInt>(blob.Size()));
		}),
		[](void* ptr) {
			Csm::CubismMoc::Delete(static_cast<Csm::CubismMoc*>(ptr));
		}
	},
	pose_{
		std::invoke([this] () -> std::optional<Handler> {
			const auto fileName = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetPoseFileName();
			if (*fileName)
			{
				const auto poseBlob = loader_((homeDir_ + fileName).c_str());
				return Handler{
					Csm::CubismPose::Create(poseBlob.Get<Csm::csmByte*>(), poseBlob.Size()),
					[](void* p) {
						Csm::CubismPose::Delete(static_cast<Csm::CubismPose*>(p));
					}
				};
			}
			else
				return std::nullopt;
		})
		},
	physicsJson_{
		std::invoke([this]() -> std::optional<Snowing::Blob> {
			const auto phyName = modelSetting_.Get<Csm::CubismModelSettingJson*>()->GetPhysicsFileName();
			if (*phyName)
				return loader_((homeDir_ + phyName).c_str());
			else
				return std::nullopt;
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
			std::string path = homeDir_ + setting->GetTextureFileName(i);
			
			// Modify Texture Extract Name
			auto iter = path.end();
			*(iter - 3) = 'c';
			*(iter - 2) = 't';
			*(iter - 1) = 'x';

			tex_[i].emplace(Snowing::Graphics::LoadTexture(loader_(path.c_str())));
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

const std::optional<Live2D::ModelAsset::Handler>& Live2D::ModelAsset::GetPose() const
{
	return pose_;
}

const std::optional<Snowing::Blob>& Live2D::ModelAsset::GetPhysicsJson() const
{
	return physicsJson_;
}
