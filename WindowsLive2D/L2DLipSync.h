#pragma once
#include "L2DModel.h"

namespace Live2D
{
	class LipSync final : public Snowing::Scene::Object,public Snowing::MemPool<LipSync>
	{
	private:
		friend class ::Live2D::Model;
		Model* model_;
		float volume_ = 0.0f;

	public:
		LipSync(Model* model);
		~LipSync();

		void SetVolume(float v);
	};
}
