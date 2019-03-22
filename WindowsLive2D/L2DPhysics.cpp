#include "L2DPhysics.h"
#include <Model/CubismModel.hpp>
#include <Physics/CubismPhysics.hpp>

Live2D::Physics::Physics(Model* model) :
	model_{
		model->GetModel().Get<Csm::CubismModel*>(),
		Snowing::Platforms::Handler::DoNothingDeleter
	},
	phys_{
		std::invoke([model]() -> decltype(phys_) {
			if (model->GetAsset()->GetPhysicsJson().has_value())
			{
				const auto& blob = model->GetAsset()->GetPhysicsJson().value();
				return Snowing::Platforms::Handler
				{
					Csm::CubismPhysics::Create(blob.Get<Csm::csmByte*>(), blob.Size()),
					[](void* p) { Csm::CubismPhysics::Delete(static_cast<Csm::CubismPhysics*>(p)); }
				};
			}
			else
				return std::nullopt;
		})
	}
{
}

void Live2D::Physics::SetParams(Params p)
{
	if (phys_.has_value())
	{
		Csm::CubismPhysics::Options phyOpt;
		phyOpt.Gravity.X = p.Gravity.x;
		phyOpt.Gravity.Y = p.Gravity.y;
		phyOpt.Wind.X = p.Wind.x;
		phyOpt.Wind.Y = p.Wind.y;
		phys_.value().Get<Csm::CubismPhysics*>()->SetOptions(phyOpt);
	}
}

bool Live2D::Physics::Update()
{
	if (phys_.has_value())
	{
		phys_.value().Get<Csm::CubismPhysics*>()->Evaluate(
			model_.Get<Csm::CubismModel*>(),
			Snowing::Engine::Get().DeltaTime());
		return true;
	}
	else
		return false;
}
