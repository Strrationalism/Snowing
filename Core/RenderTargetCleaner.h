#pragma once
#include "Object.h"
#include "Vec4.h"

namespace Snowing::Scene
{
	template <
		typename TRenderTarget,
		typename TContext,
		typename TEngine>
	class RenderTargetCleanerInterface final : public Object
	{
	private:
		TRenderTarget &rt_;
		TContext &ct_;
		const Math::Vec4f col_;
	public:
		RenderTargetCleanerInterface(
			TContext *ctx,
			TRenderTarget *rt,
			Math::Vec4f col = { 0.0f,0.0f,0.0f,0.0f }) :
			rt_{ *rt },
			ct_{ *ctx },
			col_{ col }
		{}

		bool Update() override
		{
			TEngine::Get().Draw([this] {
				ct_.ClearRenderTarget(rt_, col_);
			});

			return true;
		}
	};
}
