#pragma once
#include <Snowing.h>
#include "L2DModel.h"

namespace Live2D
{
	class Breath final : public Snowing::Scene::Object, public Snowing::MemPool<Breath>
	{
	private:
		Snowing::Platforms::Handler model_;
		Snowing::Platforms::Handler breath_;
		
	public:

		struct Params final
		{
			struct Param
			{
				float Offset, Peak, Cycle, Weight;
			};


			Snowing::Math::Vec3<Param> Angle
			{
					{ 0.0f, 15.0f, 6.5345f, 0.5f },
					{ 0.0f, 8.0f, 3.5345f, 0.5f },
					{ 0.0f, 10.0f, 5.5345f, 0.5f }
			};

			Param BodyAngleX
			{ 0.0f, 4.0f, 15.5345f, 0.5f };

			Param Breath
			{ 0.5f, 0.5f, 3.2345f, 0.5f };
		};

		Breath(Model* model, const Params& params = {});
		void SetParams(const Params& params);
		bool Update() override;
	};
}
