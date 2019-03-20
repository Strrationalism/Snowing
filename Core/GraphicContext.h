#pragma once
#include "Platforms.h"
#include "Vec4.h"
#include "Platforms.h"
#include "RenderTarget.h"
#include "GraphicBuffer.h"
#include <array>

namespace Snowing::Graphics
{
	enum class Primitive
	{
		PointList,
		TriangleList,
		TriangleStrip
	};

	template <typename TImpl>
	class[[nodiscard]] GraphicContextInterface final
	{
	private:
		TImpl impl_;

	public:
		GraphicContextInterface(GraphicContextInterface&&) = default;

		GraphicContextInterface(TImpl&& impl) :
			impl_{ std::move(impl) }
		{}

		template <typename TRenderTarget>
		void ClearRenderTarget(TRenderTarget& rt, Math::Vec4f col = { 0.0f,0.0f,0.0f,0.0f })
		{
			Platforms::AssertInterface<RenderTargetInterface>(rt);
			impl_.ClearRenderTarget(rt, col);
		}

		template <typename VertexBuffer>
		void SetVertexBuffer(Primitive pt,const VertexBuffer* vb,size_t stride,size_t offset = 0)
		{
			Platforms::AssertInterface<BufferInterface>(*vb);
			impl_.SetVertexBuffer(pt,vb, stride,offset);
		}

		void Draw(size_t count,size_t offset = 0) const
		{
			if(count)
				impl_.Draw(count,offset);
		}

		template <int N,typename TBuffer>
		void SetStreamOutBuffer(std::array<TBuffer*, N>& rt)
		{
			Platforms::AssertInterface<BufferInterface>(**rt.data());
			impl_.SetStreamOutBuffer(rt.data(), N);
		}

		void CloseStreamOut()
		{
			impl_.SetStreamOutBuffer(nullptr, 0);
		}

		template <int N,typename TRenderTarget>
		void SetRenderTarget(std::array<TRenderTarget*,N>& rt)
		{
			Platforms::AssertInterface<RenderTargetInterface>(**rt.data());
			impl_.SetRenderTarget(rt.data(), N);
		}

		template <typename TRenderTarget>
		void SetRenderTarget(TRenderTarget *rt)
		{
			Platforms::AssertInterface<RenderTargetInterface>(*rt);
			impl_.SetRenderTarget(&rt, 1);
		}

		[[nodiscard]]
		const auto& GetImpl() const
		{
			return impl_;
		}
	};
}
