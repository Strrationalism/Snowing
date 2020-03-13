#pragma once
#include "L2DModelAsset.h"

namespace Live2D
{
	class Physics;
	class LipSync;
	class Model final : public Snowing::Scene::Object, public Snowing::MemPool<Model>,Snowing::NoCopyMove
	{
	private:
		friend class ::Live2D::LipSync;
		friend class ::Live2D::Physics;
		::Live2D::LipSync* lipSync_ = nullptr;
		::Live2D::Physics* phys_ = nullptr;

		float ratio_;
		Snowing::Math::Vec2f translate_ = { 0,0 }, scale_ = { 1,1 };
		const ModelAsset* asset_;
		Snowing::Graphics::Context* ctx_;
		Snowing::Platforms::Handler model_;
		Snowing::Platforms::Handler renderer_;
		Snowing::Platforms::Handler pose_;
		
		constexpr static size_t CsmMotionManagerSize = 64;
		std::array<std::uint8_t, CsmMotionManagerSize> motionManagerBox_, expressionManagerBox_;
		Snowing::Platforms::Handler motionManager_, expressionManager_;

		void updateMatrix();
		

	public:
		Model(Snowing::Graphics::Context*,const ModelAsset* asset,float ratio);

		bool Update() override;

		static constexpr Snowing::Math::Coordinate2DCenter Coordinate
		{
			{ 0,0 },
			{ 2,2 }
		};

		bool MotionFinished() const;
		bool ExpressionFinished() const;

		void StopAllMotion();
		void StopAllExpression();

		void SetTranslate(Snowing::Math::Vec2f translate);

		template <typename TCoord>
		void SetTranslate(Snowing::Math::Vec2f translate, TCoord coord)
		{
			SetTranslate(
				Snowing::Math::ConvertPosition2DCoordinate(translate, coord, Coordinate));
		}

		void SetScale(Snowing::Math::Vec2f scale);

		const Snowing::Platforms::Handler& GetModel() const;
		const ModelAsset* GetAsset() const;

		const Snowing::Platforms::Handler& GetMotionManager() const;
		const Snowing::Platforms::Handler& GetExpressionManager() const;
		Snowing::Math::Vec2f GetCanvasSize() const;
	};
}