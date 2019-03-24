#include "L2DMotion.h"
#include <Motion/CubismMotion.hpp>
#include <Motion/CubismMotionManager.hpp>

Live2D::Motion::Motion(Model* model, size_t groupID, size_t motionID):
	model_{ model },
	motion_{
		Csm::CubismMotion::Create(
			model->GetAsset()->GetMotionGroup(groupID).second.at(motionID).Get<Csm::csmByte*>(),
			static_cast<Csm::csmSizeInt>(model->GetAsset()->GetMotionGroup(groupID).second.at(motionID).Size())),
		[](void* p) {
			Csm::CubismMotion::Delete(static_cast<Csm::CubismMotion*>(p));
		}
	}
{
}

void Live2D::Motion::SetLoop(bool b)
{
	motion_.Get<Csm::CubismMotion*>()->IsLoop(b);
}

void Live2D::Motion::Play()
{
	const auto m = model_->GetMotionManager().Get<Csm::CubismMotionManager*>();
	const auto motion = motion_.Get<Csm::CubismMotion*>();
	m->StartMotion(motion, false, 10);
}

