#include "stdafx.h"
#include "D3DContext.h"
#include "D3DRenderTarget.h"
#include "D3DBuffer.h"
#include <d3d11.h>

using namespace Snowing::PlatformImpls::WindowsImpl;

Snowing::PlatformImpls::WindowsImpl::D3D::Context::Context(Handler && h):
	context_{ std::move(h) }
{
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Context::ClearRenderTarget(Snowing::Graphics::RenderTarget & rt,Math::Vec4f col)
{
	auto pRt = 
		static_cast<ID3D11RenderTargetView*>(rt.GetImpl().GetHandler().Get<IUnknown*>());
	const FLOAT color[] = { col.x,col.y,col.z,col.w };
	static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>())->ClearRenderTargetView(pRt, color);
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Context::SetVertexBuffer(Graphics::Primitive pt, const Graphics::Buffer* vb, size_t stride, size_t offset)
{
	const auto ctx = static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>());
	switch (pt)
	{
	case Snowing::Graphics::Primitive::PointList:
		ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case Snowing::Graphics::Primitive::TriangleList:
		ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case Snowing::Graphics::Primitive::TriangleStrip:
		ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	default:
		assert(false);
		break;
	};

	const UINT uStride = static_cast<UINT>(stride);
	const UINT uOffset = static_cast<UINT>(offset);
	const auto vbo = static_cast<ID3D11Buffer*>(vb->GetImpl().GetHandler().Get<IUnknown*>());
	ctx->IASetVertexBuffers(0, 1, &vbo, &uStride, &uOffset);

}

void Snowing::PlatformImpls::WindowsImpl::D3D::Context::Draw(size_t count, size_t offset) const
{
	const auto ctx = static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>());
	ctx->Draw(static_cast<UINT>(count),static_cast<UINT>(offset));
}


void Snowing::PlatformImpls::WindowsImpl::D3D::Context::SetRenderTarget(Graphics::RenderTarget ** rt, int size)
{
	assert(size > 0);
	rt_ = *rt;
	static std::vector<ID3D11RenderTargetView*> rtCache;
	static std::vector<D3D11_VIEWPORT> vpCache;
	vpCache.clear();
	rtCache.clear();

	for (int i = 0; i < size; ++i)
	{
		rtCache.emplace_back(static_cast<ID3D11RenderTargetView*>(rt[i]->GetImpl().GetHandler().Get<IUnknown*>()));
		D3D11_VIEWPORT vp = { 0 };
		vp.MaxDepth = 1.0f;
		const auto sizeTex = rt[i]->GetTexture().Size();
		vp.Width = (float)sizeTex.x;
		vp.Height = (float)sizeTex.y;
		vpCache.push_back(vp);
	}

	const auto ctx = static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>());
	ctx->OMSetRenderTargets(size, rtCache.data(),nullptr);
	ctx->RSSetViewports(size, vpCache.data());
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Context::SetStreamOutBuffer(Graphics::Buffer** buf, int bufSize)
{
	const auto ctx = static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>());
	if (buf)
	{
		assert(bufSize < 32);
		static std::vector<ID3D11Buffer*> bufs;
		constexpr UINT offsets[32] = { 0 };
		bufs.clear();

		for (int i = 0; i < bufSize; ++i)
		{
			auto p = buf[i]->GetImpl().GetHandler().Cast<IUnknown*, ID3D11Buffer*>();
			bufs.push_back(p);
		}
		ctx->SOSetTargets(bufSize, bufs.data(), offsets);
	}
	else
		ctx->SOSetTargets(0, nullptr, nullptr);
}

Snowing::Graphics::RenderTarget* Snowing::PlatformImpls::WindowsImpl::D3D::Context::GetRenderTarget()
{
	return rt_;
}

const Handler & Snowing::PlatformImpls::WindowsImpl::D3D::Context::GetHandler() const
{
	return context_;
}

