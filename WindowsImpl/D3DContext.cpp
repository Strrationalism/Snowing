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
	const auto ctx = static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>());
	if (rt) {
		assert(size < 8);
		assert(size >= 0);
		rt_ = *rt;
		std::array<ID3D11RenderTargetView*, 8> rtCache{ 0 };
		std::array<D3D11_VIEWPORT, 8> vpCache;

		for (int i = 0; i < size; ++i)
		{
			rtCache[i] = (static_cast<ID3D11RenderTargetView*>(rt[i]->GetImpl().GetHandler().Get<IUnknown*>()));
			D3D11_VIEWPORT vp = { 0 };
			vp.MaxDepth = 1.0f;
			const auto sizeTex = rt[i]->GetTexture().Size();
			vp.Width = static_cast<float>(sizeTex.x);
			vp.Height = static_cast<float>(sizeTex.y);
			vpCache[i] = vp;
		}
		
		ctx->OMSetRenderTargets(8, rtCache.data(), nullptr);
		ctx->RSSetViewports(size, vpCache.data());
	}
	else {
		ID3D11RenderTargetView* p[8] = { nullptr };
		ctx->OMSetRenderTargets(8, p, nullptr);
		ctx->RSSetViewports(0, nullptr);
	}
}

void Snowing::PlatformImpls::WindowsImpl::D3D::Context::SetStreamOutBuffer(Graphics::Buffer** buf, int bufSize)
{
	const auto ctx = static_cast<ID3D11DeviceContext*>(context_.Get<IUnknown*>());
	if (buf)
	{
		assert(bufSize < 4);
		std::array<ID3D11Buffer*,4> bufs;
		constexpr UINT offsets[4] = { 0 };

		for (int i = 0; i < bufSize; ++i)
		{
			auto p = buf[i]->GetImpl().GetHandler().Cast<IUnknown*, ID3D11Buffer*>();
			bufs[i] = p;
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

