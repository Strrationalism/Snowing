#pragma once
#include "L2DModel.h"

namespace Live2D
{
	class LipSync final : public Snowing::Scene::Object,public Snowing::MemPool<LipSync>
	{
	private:
		Model* model_;

	public:
		LipSync(Model* model);

		void SetVolume(float v);
	};
}
