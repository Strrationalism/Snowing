#pragma once
#include "L2DModel.h"

namespace Live2D
{
	class Motion final : public Snowing::Scene::Object, public Snowing::MemPool<Motion>
	{
	private:
		Model* model_;
		Snowing::Platforms::Handler motion_;

	public:
		Motion(Model* model, size_t groupID,size_t motionID);

		void Play();
	};
}
