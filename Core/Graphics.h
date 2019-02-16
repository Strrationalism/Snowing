#pragma once
#include "Platforms.h"
#include "SingleInstance.h"
#include "GraphicContext.h"

namespace Snowing::Graphics
{
	template <typename TImpl>
	class[[nodiscard]] GraphicsInterface final : public SingleInstance<GraphicsInterface<TImpl>>
	{
	public:
		[[nodiscard]]
		static auto& MainContext()
		{
			auto& p = TImpl::Get().MainContext();
			Platforms::AssertInterface<GraphicContextInterface>(p);
			return p;
		}

		[[nodiscard]]
		static auto& MainRenderTarget()
		{
			auto& p = TImpl::Get().MainRenderTarget();
			Platforms::AssertInterface<RenderTargetInterface>(p);
			return p;
		}
	};
}
