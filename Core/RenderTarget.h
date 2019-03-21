#pragma once
#include "Platforms.h"
#include "Vec2.h"
#include "Texture2D.h"

namespace Snowing::Graphics
{
	template <typename TImpl>
	class[[nodiscard]] RenderTargetInterface final
	{
	private:
		TImpl impl_;

	public:
		RenderTargetInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{}

		RenderTargetInterface(Math::Vec2<int> size) :
			impl_{ size }
		{}

		RenderTargetInterface(RenderTargetInterface&&) = default;

		[[nodiscard]]
		const auto& GetTexture() const
		{
			auto& p = impl_.GetTexture();
			Platforms::AssertInterface<Texture2DInterface>(p);
			return p;
		}

		[[nodiscard]]
		const auto& GetImpl() const
		{
			return impl_;
		}
	};
}

