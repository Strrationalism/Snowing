#include "L2DExpression.h"
#include <Motion/CubismExpressionMotion.hpp>
#include <Motion/CubismMotionManager.hpp>

Live2D::Expression::Expression(Model* model, size_t expressionID):
	model_{ model },
	expression_{
		std::invoke([model,expressionID] {
			const auto& blob = model->GetAsset()->GetExpressions().at(expressionID).second;
			return Csm::CubismExpressionMotion::Create(blob.Get<Csm::csmByte*>(), static_cast<Csm::csmSizeInt>(blob.Size()));
		}),
		[](void* p) {
			Csm::CubismExpressionMotion::Delete(static_cast<Csm::CubismExpressionMotion*>(p));
		}
	}
{
}

void Live2D::Expression::Apply(int pri) const
{
	const auto m = model_->GetExpressionManager().Get<Csm::CubismMotionManager*>();
	const auto motion = expression_.Get<Csm::CubismExpressionMotion*>();
	m->StartMotionPriority(motion, false, pri);
}

void Live2D::Expression::SetFadeInTime(float t)
{
	auto motion = expression_.Get<Csm::CubismExpressionMotion*>();
	motion->SetFadeInTime(t);
}

void Live2D::Expression::SetFadeOutTime(float t)
{
	auto motion = expression_.Get<Csm::CubismExpressionMotion*>();
	motion->SetFadeOutTime(t);
}

float Live2D::Expression::GetWeight() const
{
	auto motion = expression_.Get<Csm::CubismExpressionMotion*>();
	return motion->GetWeight();
}

void Live2D::Expression::SetWeight(float w)
{
	auto motion = expression_.Get<Csm::CubismExpressionMotion*>();
	return motion->SetWeight(w);
}

