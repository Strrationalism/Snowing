#include "stdafx.h"
#include "D3DBuffer.h"
#include "D3DHelper.h"
#include "COMHelper.h"
#include "D3DDevice.h"

void * Snowing::PlatformImpls::WindowsImpl::D3D::Buffer::map(Snowing::Graphics::Context & c, Snowing::Graphics::AccessType acc)
{
	D3D11_MAP mt;
	switch (acc)
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
	COMHelper::AssertHResult("Can not map d3d11 buffer.",
		static_cast<ID3D11DeviceContext*>(c.GetImpl().GetHandler().Get<IUnknown*>())->
		Map(static_cast<ID3D11Buffer*>(buf_.Get<IUnknown*>()), 0, mt, 0, &data));
	return data.pData;
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Buffer::unmap(Snowing::Graphics::Context & c)
{
	static_cast<ID3D11DeviceContext*>(c.GetImpl().GetHandler().Get<IUnknown*>())->Unmap(
		static_cast<ID3D11Buffer*>(buf_.Get<IUnknown*>()), 0);
}

Snowing::PlatformImpls::WindowsImpl::D3D::Buffer::Buffer(size_t size, Snowing::Graphics::BufferBindMode mode, const void * initData, Snowing::Graphics::BufferUsage usage, Snowing::Graphics::BufferCPUAccessFlag cpuAccessFlag) :
	buf_{
		std::invoke([=]() {
		auto p = Device::Get().GetHandler().Cast<IUnknown*,ID3D11Device*>();

		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = static_cast<UINT>(size);
		desc.BindFlags = BindFlag2D3D11(mode);
		desc.Usage = Usage2D3D11(usage);
		desc.CPUAccessFlags = CPUAcc2D3D11(cpuAccessFlag);

		const D3D11_SUBRESOURCE_DATA data = { initData,0,0 };

		ID3D11Buffer* buf;
		COMHelper::AssertHResult("Can not create d3d11 buffer.",
			p->CreateBuffer(&desc, &data, &buf));
		return Handler{ static_cast<IUnknown*>(buf),COMHelper::COMIUnknownDeleter };
	}) }
{}

void Snowing::PlatformImpls::WindowsImpl::D3D::Buffer::CopyTo(Snowing::Graphics::Context & ctx, Graphics::BufferInterface<Buffer>& dst) const
{
	ctx.GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>()->CopyResource(
		dst.GetImpl().GetHandler().Cast<IUnknown*, ID3D11Buffer*>(),
		buf_.Cast<IUnknown*, ID3D11Buffer*>());
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Buffer::UpdateData(Snowing::Graphics::Context & ctx, const void * data, const std::optional<Graphics::Box<size_t>>& slice, size_t rowPitch, size_t depPitch)
{
	D3D11_BOX box;
	const D3D11_BOX* b = nullptr;

	if (slice.has_value())
	{
		box.left = static_cast<UINT>(slice->left);
		box.right = static_cast<UINT>(slice->right);
		box.back = static_cast<UINT>(slice->back);
		box.top = static_cast<UINT>(slice->top);
		box.bottom = static_cast<UINT>(slice->bottom);
		box.front = static_cast<UINT>(slice->front);
		b = &box;
	}

	ctx.GetImpl().GetHandler().Cast<IUnknown*, ID3D11DeviceContext*>()->UpdateSubresource(
		buf_.Cast<IUnknown*, ID3D11Buffer*>(), 0, b, data, static_cast<UINT>(rowPitch), static_cast<UINT>(depPitch));
}

const Snowing::PlatformImpls::WindowsImpl::Handler & Snowing::PlatformImpls::WindowsImpl::D3D::Buffer::GetHandler() const
{
	return buf_;
}
