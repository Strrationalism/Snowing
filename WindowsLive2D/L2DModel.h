#pragma once
#include "L2DModelAsset.h"

namespace Live2D
{
	class Model final : public Snowing::Scene::Object
	{
	private:
		const ModelAsset* asset_;
		Snowing::Graphics::Context* ctx_;
		Snowing::Platforms::Handler model_;
		Snowing::Platforms::Handler renderer_;
		

	public:
		Model(Snowing::Graphics::Context*,const ModelAsset* asset,float ratio);

		bool Update() override;

		const Snowing::Platforms::Handler& GetModel() const;
	};
}