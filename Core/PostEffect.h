#pragma once
#include <array>
#include "SingleInstance.h"
#include "Effect.h"
#include "GraphicBuffer.h"
#include "Vec2.h"
#include "Effect.h"

namespace Snowing::Graphics
{
	template <
		typename TBufferImpl,
		typename TEffectTechImpl,
		typename TEffectImpl,
		typename TContextImpl>
	class[[nodiscard]] PostEffectInterface final
	{
	private:
		using GPUBuffer = Graphics::BufferInterface<TBufferImpl>;
		using EffectTech = Graphics::EffectTechInterface<TEffectTechImpl,TEffectImpl>;
		using Context = Graphics::GraphicContextInterface<TContextImpl>;

	public:

		struct[[nodiscard]] alignas(4) PostEffectVertex final
		{
			Math::Vec2f Position;

			constexpr static EffectDataElement DataLayout[]
			{
				EffectDataElement::Position_Float32_2D
			};
		};
		
	private:

		static_assert(std::is_standard_layout<PostEffectVertex>::value);
		static_assert(sizeof(Math::Vec2f) == sizeof(PostEffectVertex));

		const static inline std::array<PostEffectVertex, 4> ScreenMesh
		{
			PostEffectVertex{Math::Vec2f{1.0f,1.0f}},
			PostEffectVertex{Math::Vec2f{1.0f,0.0f}},
			PostEffectVertex{Math::Vec2f{0.0f,1.0f}},
			PostEffectVertex{Math::Vec2f{0.0f,0.0f}}
		};

		EffectTech *tech_;
		Context *ctx_;

	public:
		// 使用PostEffect前必须先创建此资源
		class[[nodiscard]] PostEffectVertexBuffer final : public SingleInstance<PostEffectVertexBuffer>
		{
		private:
			GPUBuffer value_
			{
				ScreenMesh.size() * sizeof(PostEffectVertex),
				DefaultVertexBufferMode,
				static_cast<const void*>(ScreenMesh.data()),
				BufferUsage::Immutable
			};

			template <typename, typename, typename, typename>
			friend class PostEffectInterface;
		};

		PostEffectInterface(Context *ctx,EffectTech *tech) :
			ctx_{ ctx },
			tech_{ tech }
		{}

		void SetTech(EffectTech *tech)
		{
			tech_ = tech;
		}

		void Apply()
		{
			ctx_->SetVertexBuffer(
				Primitive::TriangleStrip, 
				&PostEffectVertexBuffer::Get().value_, 
				sizeof(PostEffectVertex));

			for (size_t i = 0; i < tech_->PassCount(); ++i)
			{
				(*tech_)[i](*ctx_);
				ctx_->Draw(ScreenMesh.size());
			}
		}
	};
}