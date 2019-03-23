#pragma once
#include <Snowing.h>
#include "L2DModel.h"

namespace Live2D
{
	class Physics final : public Snowing::Scene::Object, public Snowing::MemPool<Physics>
	{
	private:
		Snowing::Platforms::Handler model_;
		std::optional<Snowing::Platforms::Handler> phys_;
	public:
		struct Params
		{
			Snowing::Math::Vec2f 
				Gravity = { 0,0 },
				Wind = { 0,0 };
		};

		Physics(Model* model);

		void SetParams(Params);

		bool Update() override;
	};
}
