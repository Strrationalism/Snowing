#pragma once

#include <d3d11.h>
#include "D3DTexture2D.h"

/******** 不要在头文件里包含这个文件！***********/

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	inline DXGI_FORMAT TexFormat2DXGI(TextureFormat f)
	{
		switch (f)
		{
		case Snowing::PlatformImpls::WindowsImpl::D3D::TextureFormat::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case Snowing::PlatformImpls::WindowsImpl::D3D::TextureFormat::R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
			break;
		case Snowing::PlatformImpls::WindowsImpl::D3D::TextureFormat::B5G6R5_UNORM:
			return DXGI_FORMAT_B5G6R5_UNORM;
			break;
		default:
			throw std::invalid_argument{ "Unknown texture format." };
		}
	}

	inline DXGI_FORMAT PixFormat2DXGI(Snowing::Graphics::PixelFormat fmt)
	{
		switch (fmt)
		{
		case Graphics::PixelFormat::R8G8B8A8: return DXGI_FORMAT_R8G8B8A8_UNORM; break;
		case Graphics::PixelFormat::B5G6R5: return DXGI_FORMAT_B5G6R5_UNORM; break;
		default: throw std::invalid_argument{ "Unknown pixel format." };
		}
	}

	inline TextureFormat PixFormat2TexFormat(Graphics::PixelFormat fmt)
	{
		switch(fmt)
		{
		case Graphics::PixelFormat::R8G8B8A8: return TextureFormat::R8G8B8A8_UNORM; break;
		case Graphics::PixelFormat::B5G6R5: return TextureFormat::B5G6R5_UNORM; break;
		default: throw std::invalid_argument{ "Unknown pixel format." };;
		}
	}

	inline UINT BindFlag2D3D11(Snowing::Graphics::BufferBindMode mode)
	{
		UINT flag = 0;
		switch (mode.BindType)
		{
		case Snowing::Graphics::BufferBindMode::BindTypeEnum::None:
			flag = 0;
			break;
		case Snowing::Graphics::BufferBindMode::BindTypeEnum::VertexBuffer:
			flag = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			break;
		case Snowing::Graphics::BufferBindMode::BindTypeEnum::ConstantBuffer:
			flag = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
			break;
		case Snowing::Graphics::BufferBindMode::BindTypeEnum::ShaderResources:
			flag = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			break;
		default:
			throw std::invalid_argument{ "Unknown d3d11 buffer bind mode ." };
			break;
		};

		if (mode.StreamOutput) flag |= D3D11_BIND_FLAG::D3D11_BIND_STREAM_OUTPUT;
		if (mode.RenderTarget) flag |= D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		return flag;
	}

	inline D3D11_USAGE Usage2D3D11(Snowing::Graphics::BufferUsage usage)
	{
		switch (usage)
		{
		case Snowing::Graphics::BufferUsage::Default:
			return D3D11_USAGE::D3D11_USAGE_DEFAULT;
			break;
		case Snowing::Graphics::BufferUsage::Immutable:
			return D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			break;
		case Snowing::Graphics::BufferUsage::Staging:
			return D3D11_USAGE::D3D11_USAGE_STAGING;
			break;
		case Snowing::Graphics::BufferUsage::Dynamic:
			return D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			break;
		default:
			throw std::invalid_argument{ "Unknown d3d11 buffer usage." };
			break;
		}
	}

	inline UINT CPUAcc2D3D11(Snowing::Graphics::BufferCPUAccessFlag cpuf)
	{
		switch (cpuf)
		{
		case Snowing::Graphics::BufferCPUAccessFlag::CPUReadOnly:
			return D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
			break;
		case Snowing::Graphics::BufferCPUAccessFlag::CPUWriteOnly:
			return D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
			break;
		case Snowing::Graphics::BufferCPUAccessFlag::CPUReadWrite:
			return D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			break;
		case Snowing::Graphics::BufferCPUAccessFlag::NoAccess:
			return 0;
			break;
		default:
			throw std::invalid_argument{ "Unknown d3d11 buffer cpu access flag." };
			break;
		}
	}
}
