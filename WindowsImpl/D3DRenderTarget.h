#pragma once
#include <RenderTarget.h>
#include "D3DTexture2D.h"
#include "DataBox.h"

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class D3DRenderTarget final
	{
	private:
		
		Handler rt_;
		Graphics::Texture2D tex_;

		D3DTexture2D init(D3DTexture2D&& tex, const Handler& device);
	public:
		D3DRenderTarget(Math::Vec2<size_t> size);

		D3DRenderTarget(D3DTexture2D&& tex,const Handler& device);
		D3DRenderTarget(D3DRenderTarget&&) = default;
		D3DRenderTarget& operator = (D3DRenderTarget&&) = delete;

		const Handler& GetHandler() const;
		const Graphics::Texture2D& GetTexture() const;
	};
}

namespace Snowing::Graphics
{
	using RenderTarget = RenderTargetInterface<PlatformImpls::WindowsImpl::D3D::D3DRenderTarget>;
}

