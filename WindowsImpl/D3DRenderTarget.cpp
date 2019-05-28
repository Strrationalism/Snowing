#include "stdafx.h"
#include "D3DRenderTarget.h"
#include "D3DHelper.h"
#include "COMHelper.h"
#include "D3DDevice.h"

using namespace Snowing::PlatformImpls::WindowsImpl;

Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D Snowing::PlatformImpls::WindowsImpl::D3D::D3DRenderTarget::init(
	D3DTexture2D && tex, const Handler & device)
{
	ID3D11RenderTargetView* rt;

	COMHelper::AssertHResult(
		"Can not create render target.",
		static_cast<ID3D11Device*>(device.Get<IUnknown*>())->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(tex.GetHandler().Get<IUnknown*>()),
			nullptr,
			&rt));

	rt_ = { static_cast<IUnknown*>(rt),COMHelper::COMIUnknownDeleter };
	return std::move(tex);
}

static Snowing::Math::Vec2<uint16_t> ConvVec(Snowing::Math::Vec2<size_t> i)
{
	return 
	{
		static_cast<uint16_t>(i.x),
		static_cast<uint16_t>(i.y)
	};
}

constexpr static Snowing::Graphics::BufferBindMode RenderTargetMode =
{
	Snowing::Graphics::BufferBindMode::BindTypeEnum::ShaderResources,
	false,
	true
};

Snowing::PlatformImpls::WindowsImpl::D3D::D3DRenderTarget::D3DRenderTarget(Math::Vec2<size_t> size, Graphics::PixelFormat fmt, Graphics::BufferCPUAccessFlag cpuAccess):
	tex_{D3DTexture2D{
		fmt,
		ConvVec(size),RenderTargetMode,
		nullptr,
		Snowing::Graphics::BufferUsage::Default,
		cpuAccess}}
{
	rt_ =
		std::invoke([this,fmt]() {
		auto device = Device::Get().GetHandler().Cast<IUnknown*, ID3D11Device*>();

		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = PixFormat2DXGI(fmt);
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		ID3D11RenderTargetView* rt;
		COMHelper::AssertHResult("Can not create render target view.",
			device->CreateRenderTargetView(
				tex_.GetImpl().GetHandler().Cast<IUnknown*, ID3D11Texture2D*>(),
				&desc,
				&rt));

		return Handler{ static_cast<IUnknown*>(rt),COMHelper::COMIUnknownDeleter };
	});
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DRenderTarget::D3DRenderTarget(D3DTexture2D && tex, const Handler& device):
	tex_{ init(std::move(tex),device) }
{
}

const Handler & Snowing::PlatformImpls::WindowsImpl::D3D::D3DRenderTarget::GetHandler() const
{
	return rt_;
}

const Snowing::Graphics::Texture2D & Snowing::PlatformImpls::WindowsImpl::D3D::D3DRenderTarget::GetTexture() const
{
	return tex_;
}
