#pragma once
#include <cassert>
#include <vector>
#include <map>
#include <string_view>
#include "SpriteRenderer.h"
#include "Texture2D.h"
#include "CSVParser.h"

namespace Snowing::Graphics
{
	template <typename TSpriteSheet,typename TTexture2D,typename TBuffer>
	struct[[nodiscard]] FontInterface final
	{
		std::vector<TTexture2D> Faces;
		std::vector<TBuffer> SpriteSheets;
		std::vector<TSpriteSheet> SpriteSheetsCPU;
		std::map<wchar_t, std::pair<uint16_t, uint16_t>> Charaters;
	};

	struct[[nodiscard]] FontSprite final
	{
		Sprite Sprite;
		uint32_t FaceID = 0;

		template <typename TFont>
		inline void SetCharater(const TFont& font, wchar_t ch)
		{
			auto[faceID, spID] = font.Charaters.at(ch);
			FaceID = faceID;
			Sprite.ImageID = spID;
		};

		enum class AlignMode
		{
			Left,Right,Center
		};

		template <typename TFont>
		inline static void SetString(
			std::wstring_view sv,	//要被渲染的字符串
			const TFont& font,		//要被渲染的字体
			Math::Vec4f box,		//所在的框
			Math::Vec2f space,		//字的间距
			Math::Vec2f fontSize,	//字体大小
			const std::map<wchar_t, Math::Vec2f> &charaterFix,	//对每个的字符进行右边宽度的修正
			std::vector<FontSprite>& out,				//输出的精灵列表
			AlignMode alignMode = AlignMode::Left)		//对齐方式
		{
			out.clear();

			float xPos = 0;
			int lineNum = 0;
			size_t lineBegin = 0;
			float lineWidth = 0;

			const auto nextLine = [&](int offset) {
				xPos = 0, lineNum++;


				switch (alignMode)
				{
				case AlignMode::Left:
					for (size_t i = lineBegin; i < out.size() + offset; ++i)
						out[i].Sprite.Position.x += box.x;
					break;
				case AlignMode::Right:
					for (size_t i = lineBegin; i < out.size() + offset; ++i)
						out[i].Sprite.Position.x += box.x + box.z - lineWidth;
					break;
				case AlignMode::Center:
					for (size_t i = lineBegin; i < out.size() + offset; ++i)
						out[i].Sprite.Position.x += box.x + (box.z - lineWidth) / 2;
					break;
				default:
					assert(false);
					break;
				}
				
				lineBegin = out.size() + offset;
				lineWidth = 0;
			};

			for (size_t i = 0; i < sv.length(); ++i)
			{
				if (sv[i] == ' ')
				{
					xPos += 32 * fontSize.x * space.x;
					continue;
				}

				if (sv[i] == '\n')
				{
					nextLine(0);
					continue;
				}

				Math::Vec2f fix = { 0.0f,0.0f };
				if (charaterFix.count(sv[i]))
					fix = charaterFix.at(sv[i]);


				out.emplace_back();
				out.back().SetCharater(font, sv[i]);

				auto xMove =
					(fix.x + space.x) *
					font.SpriteSheetsCPU[out.back().FaceID].Sheet[out.back().Sprite.ImageID].z *
					font.Faces[out.back().FaceID].Size().x *
					fontSize.x;

				lineWidth = std::max(xPos + xMove, lineWidth);

				if (xPos + xMove > box.z)
				{
					nextLine(-1);
				}

				out.back().Sprite.Position = { xPos ,fix.y + box.y + lineNum * space.y * fontSize.y };
				xPos += xMove;
				out.back().Sprite.Size = fontSize;
				out.back().Sprite.Center = { 1.0f,1.0f };
			}

			nextLine(0);
		}

		inline static std::map<wchar_t, Math::Vec2f> LoadCharfixFromCSV(const Blob& b)
		{
			std::map<wchar_t, Math::Vec2f> ret;
			CSVParser<wchar_t> parser{ &b };
			
			do
			{
				const auto ch = parser.Pop().front();
				const auto x = parser.Pop<float>();
				const auto y = parser.Pop<float>();
				ret.insert(std::make_pair(ch, Math::Vec2f{ x,y }));
			} while (parser.NextLine());
			return ret;
		}
	};

	template <
		typename TContextImpl,
		typename TEffectImpl,
		typename TEffectTechImpl,
		typename TEffectPassImpl,
		typename TEffectRefObjImpl,
		typename TTexture2DImpl,
		typename TBufferImpl,
		typename TFont,
		typename TSprite,
		typename TSpriteSheet,
		size_t SpriteBufferSize = 8>
	class[[nodiscard]] FontRendererInterface final
	{
	private:
		using Context = GraphicContextInterface<TContextImpl>;
		using Effect = EffectInterface<TEffectImpl>;
		using Technique = EffectTechInterface<TEffectTechImpl>;
		using Pass = EffectPassInterface<TEffectPassImpl>;
		using GPUBuffer = BufferInterface<TBufferImpl>;

		using InnerRenderer = SpriteRendererInterface<
			TContextImpl,
			TEffectImpl,
			TEffectTechImpl,
			TEffectPassImpl,
			TEffectRefObjImpl,
			TTexture2DImpl,
			TBufferImpl,
			TSprite,
			TSpriteSheet,
			SpriteBufferSize>;

		std::vector<InnerRenderer> renderer_;

		const TFont &font_;

	public:
		template <typename TCoord>
		FontRendererInterface(
			Context *context,
			Effect *effect,
			Technique *tech,
			const TCoord& coordSystem,
			const TFont *font,
			GPUBuffer* vertexBuffer):
			font_{ *font }
		{
			assert(context);
			assert(effect);
			assert(tech);
			assert(font);
			assert(vertexBuffer);
			assert(font_.Faces.size() == font_.SpriteSheets.size());

			renderer_.reserve(font_.Faces.size());
			for (size_t i = 0; i < font_.Faces.size(); ++i)
				renderer_.emplace_back(
					context,
					effect,
					tech,
					coordSystem,
					&font_.Faces[i],
					&font_.SpriteSheets[i],
					vertexBuffer
				);
		}

		void FlushSpriteBuffer()
		{
			for (auto& r : renderer_)
				r.FlushSpriteBuffer();
		}

		void DrawToSpriteBuffer(const FontSprite& sp)
		{
			renderer_[sp.FaceID].DrawToSpriteBuffer(sp.Sprite);
		}

		void DrawToSpriteBuffer(const std::vector<FontSprite>& sps)
		{
			for (const auto& p : sps)
				DrawToSpriteBuffer(p);
		}

		const TFont& GetFont() const
		{
			return font_;
		}

		static constexpr auto MakeGPUVertexBuffer = InnerRenderer::MakeGPUVertexBuffer;
	};
}
