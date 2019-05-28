#pragma once
#include "Handler.h"
#include "Texture2D.h"
#include <Blob.h>
#include <optional>
#include "Vec4.h"
#include <GraphicContext.h>

namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	class Context;
	class Buffer;
	class D3DTexture2D;
}

namespace Snowing::Graphics
{
	using Context = GraphicContextInterface<PlatformImpls::WindowsImpl::D3D::Context>;
	using Buffer = BufferInterface<PlatformImpls::WindowsImpl::D3D::Buffer>;
	using Texture2D = Texture2DInterface<Snowing::PlatformImpls::WindowsImpl::D3D::D3DTexture2D>;
}


namespace Snowing::PlatformImpls::WindowsImpl::D3D
{
	enum class TextureFormat : std::uint8_t
	{
		R8G8B8A8_UNORM = 0,
		R8_UNORM = 1,
		DDS = 2,
		B5G6R5_UNORM = 3
	};

	size_t FormatPixleSize(TextureFormat f);

	class D3DTexture2D final
	{
	private:
		using CPUAccessFlag = Graphics::BufferCPUAccessFlag;
		using BufferUsage = Graphics::BufferUsage;
		mutable Handler shaderRes_;
		Handler tex_;

		std::pair<void*,size_t> map(Snowing::Graphics::Context& ctx, Snowing::Graphics::AccessType accessType);
		void unmap(Snowing::Graphics::Context& ctx);

	public:
		D3DTexture2D(
			Graphics::PixelFormat fmt,
			Math::Vec2<size_t> size,
			Graphics::BufferBindMode mode,
			void* pixels = nullptr,
			BufferUsage usage = BufferUsage::Immutable,
			CPUAccessFlag cpuAccessFlag = CPUAccessFlag::NoAccess);

		D3DTexture2D(
			TextureFormat fmt,
			Math::Vec2<std::uint16_t> size,
			Graphics::BufferBindMode mode,
			void* pixels = nullptr,
			BufferUsage usage = BufferUsage::Immutable,
			CPUAccessFlag cpuAccessFlag = CPUAccessFlag::NoAccess);

		D3DTexture2D(Handler&& hnd);

		D3DTexture2D& operator = (D3DTexture2D&&) = delete;
		D3DTexture2D(D3DTexture2D&&) = default;
		Math::Vec2<size_t> Size() const;

		const Handler& GetHandler() const;
		const Handler& ShaderResource() const;

		void CopyTo(Snowing::Graphics::Context& ctx, Snowing::Graphics::Texture2D& dst) const;

		template <typename TFunc>
		void Access(Snowing::Graphics::Context& ctx, Snowing::Graphics::AccessType accessType, TFunc& func)
		{
			auto[data,pitch] = map(ctx,accessType);
			func(data,pitch);
			unmap(ctx);
		}
	};

	constexpr size_t MaxSpriteRects = 512;
}

namespace Snowing::Graphics
{
	constexpr size_t MaxSpriteRects = Snowing::PlatformImpls::WindowsImpl::D3D::MaxSpriteRects;
	struct SpriteSheet
	{
		Math::Vec4f Sheet[MaxSpriteRects];
	};

	Texture2D LoadSpriteAndSpriteSheet(const Snowing::Blob& b,SpriteSheet& sheet);
	Texture2D LoadTexture(const Snowing::Blob& b);
}
