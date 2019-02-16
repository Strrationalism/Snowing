#pragma once
#include <optional>
#include <Engine.h>
#include <GraphicBuffer.h>
#include "Handler.h"
#include "D3DContext.h"

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class Buffer final
	{
	private:
		Handler buf_;

		void* map(Snowing::Graphics::Context&, Snowing::Graphics::AccessType acc);
		void unmap(Snowing::Graphics::Context&);

	public:
		Buffer(
			size_t size,
			Snowing::Graphics::BufferBindMode mode,
			const void* initData,
			Snowing::Graphics::BufferUsage usage,
			Snowing::Graphics::BufferCPUAccessFlag cpuAccessFlag);

		template <typename TFunc>
		void Access(Snowing::Graphics::Context& ctx,Snowing::Graphics::AccessType acc,TFunc& func)
		{
			func(map(ctx,acc));
			unmap(ctx);
		}

		void CopyTo(Snowing::Graphics::Context& ctx, Graphics::BufferInterface<Buffer>& dst) const;
		void UpdateData(
			Snowing::Graphics::Context& ctx, 
			const void* data, 
			const std::optional<Graphics::Box<size_t>>& slice,
			size_t rowPitch,
			size_t depPitch);

		const Handler& GetHandler() const;
	};
}

namespace Snowing::Graphics
{
	using Buffer = BufferInterface<Snowing::PlatformImpls::WindowsImpl::D3D::Buffer>;
}
