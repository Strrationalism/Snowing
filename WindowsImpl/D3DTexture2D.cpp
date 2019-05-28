#include "stdafx.h"
#include "D3DTexture2D.h"
#include "COMHelper.h"
#include "D3DHelper.h"
#include <cassert>
#include "D3DDevice.h"
#include "DDSTextureLoader.h"
#include "D3DBuffer.h"

using namespace Snowing;
using namespace Snowing::Graphics;
using namespace Snowing::PlatformImpls::WindowsImpl;
using namespace D3D;


std::pair<void*, size_t> D3DTexture2D::map(Snowing::Graphics::Context& ctx, Snowing::Graphics::AccessType accessType)
{
	D3D11_MAP mt;
	switch (accessType)
	{
	case Snowing::Graphics::AccessType::Read:
		mt = D3D11_MAP::D3D11_MAP_READ;
		break;
	case Snowing::Graphics::AccessType::Write:
		mt = D3D11_MAP::D3D11_MAP_WRITE;
		break;
	case Snowing::Graphics::AccessType::ReadWrite:
		mt = D3D11_MAP::D3D11_MAP_READ_WRITE;
		break;
	case Snowing::Graphics::AccessType::WriteDiscard:
		mt = D3D11_MAP::D3D11_MAP_WRITE_DISCARD;
		break;
	default:
		throw std::invalid_argument{ "Unknown d3d11 buffer access type." };
		break;
	};

	D3D11_MAPPED_SUBRESOURCE data;

	COMHelper::AssertHResult("Can not map texture data.",
		ctx.GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>()->Map(
		tex_.Cast<IUnknown*, ID3D11Texture2D*>(),
		0,
		mt,
		0,
		&data));

	return { data.pData,data.RowPitch };
}

void D3DTexture2D::unmap(Snowing::Graphics::Context& ctx)
{
	ctx.GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>()->Unmap(
		tex_.Cast<IUnknown*, ID3D11Texture2D*>(),
		0
	);
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D::D3DTexture2D(PixelFormat fmt, Math::Vec2<size_t> size, Graphics::BufferBindMode mode,
	void* pixels, BufferUsage usage, CPUAccessFlag cpuAccessFlag):
	D3DTexture2D(PixFormat2TexFormat(fmt), { static_cast<uint16_t>(size.x),static_cast<uint16_t>(size.y) }, mode, pixels, usage, cpuAccessFlag)
{
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D::D3DTexture2D(TextureFormat f, Math::Vec2<std::uint16_t> size, Graphics::BufferBindMode mode, void * pixels, BufferUsage usage, CPUAccessFlag cpuAccessFlag):
	tex_{
		std::invoke([=]() {

		auto p = Device::Get().GetHandler().Cast<IUnknown*, ID3D11Device*>();
		switch (f)
		{
			case Snowing::PlatformImpls::WindowsImpl::D3D::TextureFormat::DDS:
			{
				auto size = static_cast<size_t>(*(uint32_t*)pixels);
				auto pDDS = static_cast<uint8_t*>(pixels) + sizeof(uint32_t);
				ID3D11Resource *tex;
				ID3D11ShaderResourceView *shaderRes;
				COMHelper::AssertHResult("Can not create texture from DDS.",
					DirectX::CreateDDSTextureFromMemory(p, pDDS, size, &tex, &shaderRes));
				
				shaderRes_ = { static_cast<IUnknown*>(shaderRes),COMHelper::COMIUnknownDeleter };
				return Handler{ static_cast<IUnknown*>(tex),COMHelper::COMIUnknownDeleter };
			}
			default:
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = static_cast<UINT>(size.x);
				desc.Height = static_cast<UINT>(size.y);
				desc.Format = TexFormat2DXGI(f);
				desc.BindFlags = BindFlag2D3D11(mode);
				desc.CPUAccessFlags = CPUAcc2D3D11(cpuAccessFlag);
				desc.Usage = Usage2D3D11(usage);
				desc.ArraySize = 1;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.MipLevels = 1;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				D3D11_SUBRESOURCE_DATA* pData = nullptr;
				data.pSysMem = pixels;
				if (pixels)
				{
					pData = &data;
					data.SysMemPitch = UINT(size.x * FormatPixleSize(f));
					data.SysMemSlicePitch = data.SysMemPitch * desc.Height;
				}
				ID3D11Texture2D* tex;
				
				COMHelper::AssertHResult("Create texture2D failed.",
					p->CreateTexture2D(&desc, pData, &tex));
				return Handler{ static_cast<IUnknown*>(tex),COMHelper::COMIUnknownDeleter };
			}
		};
		
	}) }
{
}

Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D::D3DTexture2D(Handler && hnd):
	tex_{ std::move(hnd) }
{
}

Snowing::Math::Vec2<size_t> Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D::Size() const
{
	D3D11_TEXTURE2D_DESC desc;
	static_cast<ID3D11Texture2D*>(tex_.Get<IUnknown*>())->GetDesc(&desc);
	return { desc.Width,desc.Height };
}

const Handler & Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D::GetHandler() const
{
	return tex_;
}

const Handler & Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D::ShaderResource() const
{
	if (!shaderRes_.IsSome())
	{
		shaderRes_ = 
		std::invoke([this]{
			auto tex = tex_.Cast<IUnknown*, ID3D11Texture2D*>();
			D3D11_TEXTURE2D_DESC tdesc;
			tex->GetDesc(&tdesc);

			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = tdesc.Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;

			ID3D11ShaderResourceView* srv;
			COMHelper::AssertHResult("Can not create SRV.",
				Device::Get().GetHandler().Cast<IUnknown*, ID3D11Device*>()->
				CreateShaderResourceView(tex, &desc, &srv));

			return Handler{ static_cast<IUnknown*>(srv),COMHelper::COMIUnknownDeleter };
		});
	}
	return shaderRes_;
}

void D3DTexture2D::CopyTo(Snowing::Graphics::Context& ctx, Snowing::Graphics::Texture2D& dst) const
{
	ctx.GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>()->CopyResource(
		dst.GetImpl().GetHandler().Cast<IUnknown*, ID3D11Texture2D*>(),
		tex_.Cast<IUnknown*, ID3D11Texture2D*>()
	);
}

size_t Snowing::PlatformImpls::WindowsImpl::D3D::FormatPixleSize(TextureFormat f)
{
	switch (f)
	{
	case Snowing::PlatformImpls::WindowsImpl::D3D::TextureFormat::R8G8B8A8_UNORM:
		return 4;
	case Snowing::PlatformImpls::WindowsImpl::D3D::TextureFormat::R8_UNORM:
		return 1;
	default:
		throw std::invalid_argument{ "Unknown texture size,can not get pixel size." };
	}
}

Texture2D Snowing::Graphics::LoadSpriteAndSpriteSheet(const Snowing::Blob& blob, SpriteSheet & sheet)
{
	const auto size = *blob.Get<Math::Vec2<std::uint16_t>*>(sizeof(TextureFormat));
	const auto spriteCount = *blob.Get<uint16_t*>(sizeof(TextureFormat) + sizeof(Math::Vec2<uint16_t>));
	const auto sprites = blob.Get<Math::Vec4<float>*>(sizeof(TextureFormat) + sizeof(Math::Vec2<uint16_t>) + sizeof(uint16_t));


	assert(spriteCount <= MaxSpriteRects);
	std::copy(sprites, sprites + spriteCount, sheet.Sheet);

	return LoadTexture(blob);
}

Texture2D Snowing::Graphics::LoadTexture(const Snowing::Blob & blob)
{
	const auto format = *blob.Get<TextureFormat*>(0);
	const auto size = *blob.Get<Math::Vec2<std::uint16_t>*>(sizeof(format));
	const auto spriteCount = *blob.Get<uint16_t*>(sizeof(TextureFormat) + sizeof(Math::Vec2<uint16_t>));
	const auto pixel = blob.Get<void*>(
		sizeof(format) +
		sizeof(Math::Vec2<uint16_t>) +
		sizeof(uint16_t) +
		spriteCount * sizeof(Math::Vec4 <float>));

	Snowing::Graphics::BufferBindMode mode;
	mode.BindType = Snowing::Graphics::BufferBindMode::BindTypeEnum::ShaderResources;
	return D3DTexture2D{ format,size,mode,pixel };
}
