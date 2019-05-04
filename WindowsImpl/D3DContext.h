#pragma once
#include "Handler.h"
#include "D3DRenderTarget.h"
#include <GraphicBuffer.h>
#include <GraphicContext.h>

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class Buffer;
}

namespace Snowing::Graphics
{
	using Buffer = BufferInterface<Snowing::PlatformImpls::WindowsImpl::D3D::Buffer>;
}

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class Context final
	{
	private:
		Handler context_;

		Graphics::RenderTarget* rt_;
	public:
		Context(Handler&&);
		Context(Context&&) = default;
		Context& operator = (Context&&) = delete;

		void ClearRenderTarget(Graphics::RenderTarget& rt,Math::Vec4f col);
		void SetVertexBuffer(Graphics::Primitive pt,const Graphics::Buffer* vb, size_t stride, size_t offset);
		void Draw(size_t count,size_t offset) const;
		void SetRenderTarget(Graphics::RenderTarget** rt, int size);
		void SetStreamOutBuffer(Graphics::Buffer** buf, int bufSize);
		Graphics::RenderTarget* GetRenderTarget();

		const Handler& GetHandler() const;
	};
}

namespace Snowing::Graphics
{
	using Context = GraphicContextInterface<PlatformImpls::WindowsImpl::D3D::Context>;
}
