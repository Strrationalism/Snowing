#include "L2DModel.h"
#include <CubismModelSettingJson.hpp>
#include <Model/CubismMoc.hpp>

Live2D::Model::Model(const char* homeDir, const char* modelJson, Live2D::AssetLoader loader) :
	loader_{ loader },
	homeDir_{ homeDir },
	modelSetting_{
		std::invoke([this,modelJson] {
			const auto blob = loader_((homeDir_ + modelJson).c_str());

			constexpr size_t CubismModelSettingJsonSizeInner = sizeof(Csm::CubismModelSettingJson);
			static_assert(CubismModelSettingJsonSize == CubismModelSettingJsonSizeInner);
			
			return new (modelSettingBox_.data()) Csm::CubismModelSettingJson(blob.Get<Csm::csmByte*>(),blob.Size());
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
			return Csm::CubismMoc::Create(blob.Get<Csm::csmByte*>(),blob.Size());
		}),
		[](void* ptr) {
			Csm::CubismMoc::Delete(static_cast<Csm::CubismMoc*>(ptr));
		}
	}
{
	assert(homeDir_.back() == '/');

	// Json
	const auto setting = modelSetting_.Get<Csm::CubismModelSettingJson*>();

	// Load Textures
	{
		const size_t texCount = setting->GetTextureCount();
		assert(texCount <= MaxTextureCounts);
		for (size_t i = 0; i < texCount; ++i)
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
