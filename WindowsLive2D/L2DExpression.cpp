#include "L2DExpression.h"
#include <Motion/CubismExpressionMotion.hpp>
#include <Motion/CubismMotionManager.hpp>

Live2D::Expression::Expression(Model* model, size_t expressionID):
	model_{ model },
	expression_{
		std::invoke([model,expressionID] {
			const auto& blob = model->GetAsset()->GetExpressionsJson().at(expressionID).second;
			return Csm::CubismExpressionMotion::Create(blob.Get<Csm::csmByte*>(), blob.Size());
		}),
		[](void* p) {
			Csm::CubismExpressionMotion::Delete(static_cast<Csm::CubismExpressionMotion*>(p));
		}
	}
{
}

void Live2D::Expression::Apply() const
{
	const auto m = model_->GetExpressionManager().Get<Csm::CubismMotionManager*>();
	const auto motion = expression_.Get<Csm::CubismExpressionMotion*>();
	m->StartMotion(motion, false, 1);
}

