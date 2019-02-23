#pragma once
#include <array>
#include <cstdint>
#include <algorithm>
#include "Coordinate2D.h"
#include "GraphicContext.h"
#include "GraphicBuffer.h"
#include "Effect.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Texture2D.h"

namespace Snowing::Graphics
{
	struct[[nodiscard]] alignas(4) Sprite final
	{
		Math::Vec2f Position = { 0.0f,0.0f };
		Math::Vec2f Size = { 1.0f,1.0f };
		Math::Vec4f Color = { 1.0f,1.0f,1.0f,1.0f };
		Math::Vec2f Center = { 0.5f,0.5f };
		float Rotating = 0.0f;
		uint32_t ImageID = 0;

		constexpr static EffectDataElement DataLayout[] =
		{
			EffectDataElement::Position_Float32_2D,
			EffectDataElement::Size_Float32_2D,
			EffectDataElement::Color_Float32_4D,
			EffectDataElement::Center_Float32_2D,
			EffectDataElement::Rotating_Float32,
			EffectDataElement::SpriteImageIndex_UInt32
		};
	};

	static_assert(std::is_standard_layout<Sprite>::value);

	template <
		typename TContextImpl,
		typename TEffectImpl,
		typename TEffectTechImpl,
		typename TEffectPassImpl,
		typename TEffectRefObjImpl,
		typename TTexture2DImpl,
		typename TBufferImpl,
		typename TSprite,
		typename TSpriteSheet,
		size_t SpriteBufferSize = 8>
		class[[nodiscard]] SpriteRendererInterface final
	{
	private:
		using Context = GraphicContextInterface<TContextImpl>;
		using Effect = EffectInterface<TEffectImpl>;
		using Technique = EffectTechInterface<TEffectTechImpl,TEffectImpl>;
		using Pass = EffectPassInterface<TEffectPassImpl>;
		using GPUBuffer = BufferInterface<TBufferImpl>;
		using Texture = Texture2DInterface<TTexture2DImpl>;
		using Reflection = EffectReflectionObjectInterface<TEffectRefObjImpl>;

		const Texture *tex_;

		Effect &fx_;
		Technique &tech_;
		
		Context &ctx_;

		std::array<TSprite, SpriteBufferSize> buffer_;
		size_t bufferPointer_ = 0;

		GPUBuffer *gpuVBuffer_;

		const GPUBuffer* spriteSheet_;

		Math::Coordinate2DCenter coord_ = { {0,0},{1,1} };

		Reflection
			coordSystemRef_,
			spriteSheetRef_,
			spriteTexRef_,
			texSizeRef_;

		static_assert(std::is_standard_layout<TSprite>::value,"Sprite must is a standard layout type.");
		static_assert(std::is_standard_layout<TSpriteSheet>::value,"SpriteSheet must is a standard layout type.");

		void drawBuffer(size_t drawSize)
		{
			coordSystemRef_.SetAsVec4((Math::Vec4f&)coord_);
			spriteSheetRef_.SetAsConstantBuffer(spriteSheet_);

			if (tex_)
			{
				spriteTexRef_.SetAsTexture2D(tex_);
				texSizeRef_.SetAsVec2({
					static_cast<float>(tex_->Size().x),
					static_cast<float>(tex_->Size().y)
					});
			}
			else
			{
				texSizeRef_.SetAsVec2({
					static_cast<float>(1.0f),
					static_cast<float>(1.0f)
					});
			}


			for (size_t i = 0; i < tech_.PassCount(); ++i)
			{
				tech_[i](ctx_);
				ctx_.Draw(drawSize);
			}
		}

	public:
		void SetCoordinateSystem(const Math::Coordinate2DCenter& c)
		{
			coord_ = c;
		}

		void SetCoordinateSystem(const Math::Coordinate2DRect& c)
		{
			SetCoordinateSystem(ConvertCoordnate2DToCenter(c));
		}

		template <typename TCoord>
		SpriteRendererInterface(
			Context *context,
			Technique *tech,
			const TCoord& coordSystem,
			const Texture* tex,
			const GPUBuffer* spriteSheet,
			GPUBuffer* vertexBuffer) :
			tech_{ *tech },
			fx_{ *(tech->GetEffect()) },
			tex_{tex},
			ctx_{ *context },
			spriteSheet_{ spriteSheet },
			gpuVBuffer_{ vertexBuffer },
			coordSystemRef_{fx_.Reflection("CoordinateSystemCenter")},
			spriteSheetRef_{fx_.Reflection("SpriteSheet")},
			spriteTexRef_{fx_.Reflection("SpriteTexture")},
			texSizeRef_{fx_.Reflection("TextureSize")}
		{
			assert(context);
			assert(tech);
			assert(spriteSheet);

			if (gpuVBuffer_)
			{
				assert(gpuVBuffer_->Size() >= sizeof(TSprite) * SpriteBufferSize);
			}

			SetCoordinateSystem(coordSystem);
		}

		void FlushSpriteBuffer()
		{
			assert(gpuVBuffer_);
			assert(gpuVBuffer_->Size() >= sizeof(TSprite) * SpriteBufferSize);
			if (bufferPointer_ && gpuVBuffer_)
			{
				const std::optional<Snowing::Graphics::Box<size_t>> dataBox =
					Snowing::Graphics::Box<size_t>
				{
					0, bufferPointer_ * sizeof(TSprite),
					0,1,
					0,1
				};

				gpuVBuffer_->UpdateData(ctx_, buffer_.data(), dataBox);
				ctx_.SetVertexBuffer(Primitive::PointList, gpuVBuffer_, sizeof(TSprite));

				drawBuffer(bufferPointer_);

				bufferPointer_ = 0;
			}
		}

		void Draw(const TSprite *sprites, size_t count)
		{
			if (count)
			{
				assert(gpuVBuffer_);
				const auto bufferSize = gpuVBuffer_->Size() / sizeof(TSprite);
				for (size_t i = 0; i < count; i += bufferSize)
				{
					const size_t drawCount = std::clamp(count - i, size_t{ 0u }, bufferSize);
					const std::optional<Snowing::Graphics::Box<size_t>> dataBox =
						Snowing::Graphics::Box<size_t>
					{
						0, drawCount * sizeof(TSprite),
						0,1,
						0,1
					};

					gpuVBuffer_->UpdateData(ctx_, sprites + i, dataBox);
					ctx_.SetVertexBuffer(Primitive::PointList, gpuVBuffer_, sizeof(TSprite));
					drawBuffer(drawCount);
				}
			}
		}

		void Draw(const GPUBuffer& vertexBuffer, size_t count)
		{
			if (count)
			{
				ctx_.SetVertexBuffer(Primitive::PointList, &vertexBuffer, sizeof(TSprite));
				drawBuffer(count);
			}
		}

		void DrawToSpriteBuffer(const TSprite& sp)
		{
			assert(gpuVBuffer_);
			assert(gpuVBuffer_->Size() >= sizeof(TSprite) * SpriteBufferSize);
			buffer_[bufferPointer_++] = sp;
			if (bufferPointer_ >= SpriteBufferSize)
				FlushSpriteBuffer();
		}

		[[nodiscard]]
		static GPUBuffer MakeGPUSpriteSheet(const TSpriteSheet& sheet)
		{
			return
			{
				sizeof(TSpriteSheet),
				DefaultConstantBufferMode,
				(const void*)&sheet,
				BufferUsage::Immutable
			};
		}

		[[nodiscard]]
		static GPUBuffer MakeGPUVertexBuffer()
		{
			TSprite initData[SpriteBufferSize];
			return 
			{
				SpriteBufferSize * sizeof(TSprite),
				DefaultVertexBufferMode,
				initData,
				BufferUsage::Default
			};
		}
	};
}
