#pragma once
#include "L2DModelAsset.h"

namespace Live2D
{
	class Model final : public Snowing::Scene::Object
	{
	private:
		float ratio_;
		Snowing::Math::Vec2f translate_ = { 0,0 }, scale_ = { 1,1 };
		const ModelAsset* asset_;
		Snowing::Graphics::Context* ctx_;
		Snowing::Platforms::Handler model_;
		Snowing::Platforms::Handler renderer_;
		Snowing::Platforms::Handler pose_;
		
		

		void updateMatrix();
		

	public:
		Model(Snowing::Graphics::Context*,const ModelAsset* asset,float ratio);

		bool Update() override;

		void SetTranslate(Snowing::Math::Vec2f translate);
		void SetScale(Snowing::Math::Vec2f scale);

		const Snowing::Platforms::Handler& GetModel() const;
		const ModelAsset* GetAsset() const;
	};
}