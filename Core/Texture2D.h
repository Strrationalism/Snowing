#pragma once
#include "Vec2.h"
#include "Vec4.h"
#include "BKDRHash.h"
#include "Platforms.h"

namespace Snowing::Graphics
{
	template <typename TImpl>
	class[[nodiscard]] Texture2DInterface final
	{
	private:
		TImpl impl_;

	public:
		Texture2DInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{}

		[[nodiscard]]
		Math::Vec2<size_t> Size() const
		{
			return impl_.Size();
		}

		[[nodiscard]]
		const TImpl& GetImpl() const
		{
			return impl_;
		}
	};
}
