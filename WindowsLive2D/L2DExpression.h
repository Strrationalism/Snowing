#pragma once
#include "L2DModel.h"

namespace Live2D
{
	class Expression final : public Snowing::Scene::Object,public Snowing::MemPool<Expression>
	{
	private:
		const Model* model_;
		Snowing::Platforms::Handler expression_;

	public:
		Expression(Model* model, size_t expressionID);

		void Apply() const;
	};
}
