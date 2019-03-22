#pragma once
#include <Snowing.h>
#include "L2DModel.h"

namespace Live2D
{
	class Blink final : public Snowing::Scene::Object
	{
	private:
		Snowing::Platforms::Handler model_, blink_;

	public:
		Blink(Model* model, float blinkInterval = 1.0f);
		void SetBlinkInterval(float);

		bool Update() override;
	};
}
