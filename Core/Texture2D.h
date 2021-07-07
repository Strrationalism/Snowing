#pragma once
#include "Vec2.h"
#include "PixelFormat.h"
#include "GraphicBuffer.h"

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

		Texture2DInterface(
			PixelFormat fmt,
			Snowing::Math::Vec2<size_t> size, 
			Graphics::BufferBindMode mode,
			const void* pixels = nullptr, 
			BufferUsage usage = BufferUsage::Immutable,
			BufferCPUAccessFlag cpuAccess = BufferCPUAccessFlag::NoAccess) :
			impl_ {fmt,size,mode,pixels,usage,cpuAccess}
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

		template <typename TContext, typename TFunc>
		void Access(TContext& ctx, Snowing::Graphics::AccessType accessType, TFunc& func)
		{
			impl_.Access(ctx, accessType, func);
		}

		template <typename TContext,typename TDst>
		void CopyTo(TContext& context,TDst& dst) const
		{
			impl_.CopyTo(context, dst);
		}
	};
}
